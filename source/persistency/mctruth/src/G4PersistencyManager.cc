//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// File: G4PersistencyManager.cc
//
// History:
//   01.07.17  Youhei Morita  Initial creation (with "fadsclass")

#include "G4PersistencyManager.hh"

// Addtional Include:
#include <iomanip>
#include "G4PersistencyCenter.hh"

// Implementation of Constructor #1
G4PersistencyManager::G4PersistencyManager(G4PersistencyCenter* pc, std::string n)
 : f_pc(pc), nameMgr(n), f_is_initialized(false)
{
  m_verbose = f_pc->VerboseLevel();
}

// Implementation of Destructor #1
G4PersistencyManager::~G4PersistencyManager()
{}

// Implementation of GetPersistencyManager
G4PersistencyManager* G4PersistencyManager::GetPersistencyManager()
{
  return G4PersistencyCenter::GetPersistencyCenter()->CurrentPersistencyManager();
}

// Implementation of SetVerboseLevel
void G4PersistencyManager::SetVerboseLevel(int v)
{
  m_verbose = v;
  if ( m_verbose > 2 ) {
    G4cout << "G4PersistencyManager[\"" << nameMgr << "\"," << this
           << "]: verbose level is set to " << m_verbose << "."
           << G4endl;
  }
  if (   EventIO() != 0 )   EventIO()->SetVerboseLevel(m_verbose);
  if ( MCTruthIO() != 0 ) MCTruthIO()->SetVerboseLevel(m_verbose);
  if (     HitIO() != 0 )     HitIO()->SetVerboseLevel(m_verbose);
  if (   DigitIO() != 0 )   DigitIO()->SetVerboseLevel(m_verbose);
  if (TransactionManager() != 0) TransactionManager()->SetVerboseLevel(m_verbose);

  size_t i;

  G4HCIOcatalog* hcio = G4HCIOcatalog::GetHCIOcatalog();
  if ( hcio != 0 ) {
    hcio->SetVerboseLevel(m_verbose);
    for ( i = 0; i < hcio->NumberOfHCIOmanager(); i++ ) {
      hcio->GetHCIOmanager(i)->SetVerboseLevel(m_verbose);
    }
  }
  G4DCIOcatalog* dcio = G4DCIOcatalog::GetDCIOcatalog();
  if ( dcio != 0 ) {
    dcio->SetVerboseLevel(m_verbose);
    for ( i = 0; i < dcio->NumberOfDCIOmanager(); i++ ) {
      dcio->GetDCIOmanager(i)->SetVerboseLevel(m_verbose);
    }
  }
}

// Implementation of Store
G4bool G4PersistencyManager::Store(const G4Event* evt)
{
  if ( m_verbose > 2 ) {
    G4cout << "G4PersistencyManager::Store() is called for event# "
           << evt->GetEventID() << "." << G4endl;
  }

  if ( TransactionManager() == 0 ) return true;

  G4bool is_store = f_pc->CurrentStoreMode("MCTruth") != kOff ||
                    f_pc->CurrentStoreMode("Hits")    != kOff ||
                    f_pc->CurrentStoreMode("Digits")  != kOff;

  if ( ! is_store ) return true;

  // Call package dependent Initialize()
  //
  if ( ! f_is_initialized ) {
    f_is_initialized = true;
    if ( m_verbose > 1 ) {
      G4cout << "G4PersistencyManager:: Initializing Transaction ... "
             << G4endl;
    }
    Initialize();
  }

  G4bool st1 = true, st2 = true;

  // Start event IO transaction
  //
  if ( TransactionManager()->StartUpdate() ) {
    if ( m_verbose > 2 ) {
      G4cout << "G4PersistencyManager: Update transaction started for event#"
             << evt->GetEventID() << "." << G4endl;
    }
  } else {
    G4cerr << "TransactionManager::Store(G4Event) - StartUpdate() failed."
           << G4endl;
    return false;
  }

  std::string file;
  std::string obj;

  G4bool stmct = true, st3 = true;

  // Store MCTruth event
  //
  obj = "MCTruth";
  G4MCTEvent* mctevt = 0;
  if ( f_pc->CurrentStoreMode(obj) == kOn ) {

    //  Note: This part of code will not be activated until a method
    //  to obtain the current pointer of G4MCTEvent* become available.

    // if ( (mctevt = f_MCTman->GetCurrentEvent()) != 0 ) {
    if ( mctevt != 0 ) {
      file = f_pc->CurrentWriteFile(obj);
      if ( TransactionManager()->SelectWriteFile(obj, file) ) {
        stmct = MCTruthIO()->Store(mctevt);
        if ( stmct && m_verbose > 1 ) {
          G4cout << " -- File : " << file << " -- Event# "
                 << evt->GetEventID() << " -- G4MCTEvent Stored." << G4endl;
        }
      } else {
        stmct = false;
      }
    } // end of if ( mctevt != 0 )
  }

  // Store hits collection
  //
  obj = "Hits";
  if ( f_pc->CurrentStoreMode(obj) == kOn ) {
    if ( G4HCofThisEvent* hc = evt->GetHCofThisEvent() ) {
      file = f_pc->CurrentWriteFile(obj);
      if ( TransactionManager()->SelectWriteFile(obj, file) ) {
        st1 = HitIO()->Store(hc);
        if ( st1 && m_verbose > 1 ) {
          G4cout << " -- File : " << file << " -- Event# "
                 << evt->GetEventID()
                 << " -- Hit Collections Stored." << G4endl;
        }
      } else {
        st1 = false;
      }
    }
  }

  // Store digits collection
  //
  obj = "Digits";
  if ( f_pc->CurrentStoreMode(obj) == kOn ) {
    if ( G4DCofThisEvent* dc = evt->GetDCofThisEvent() ) {
      file = f_pc->CurrentWriteFile(obj);
      if ( TransactionManager()->SelectWriteFile(obj, file) ) {
        st2 = DigitIO()->Store(dc);
        if ( st2 && m_verbose > 1 ) {
          G4cout << " -- File : " << file << " -- Event# "
                 << evt->GetEventID()
                 << " -- Digit Collections Stored." << G4endl;
        }
      } else {
        st2 = false;
      }
    }
  }

  // Store this G4EVENT
  //
  if ( mctevt!=0 || evt!=0 ) {
    obj = "Hits";
    file = f_pc->CurrentWriteFile(obj);
    if ( TransactionManager()->SelectWriteFile(obj, file) ) {
      st3 = EventIO()->Store(evt);
      if ( st3 && m_verbose > 1 ) {
        G4cout << " -- File name: " << f_pc->CurrentWriteFile("Hits")
               << " -- Event# "  << evt->GetEventID()
               << " -- G4Pevent is Stored." << G4endl;
      }
    } else {
      st3 = false;
    }
  }
  
  G4bool st = stmct && st1 && st2 && st3;

  if ( st ) {
    TransactionManager()->Commit();
    if ( m_verbose > 0 )
      G4cout << "G4PersistencyManager: event# "
             << evt->GetEventID() << " is stored." << G4endl;
  } else {
    G4cerr << "G4PersistencyManager::Store(G4Event) - Transaction aborted."
           << G4endl;
    TransactionManager()->Abort();
  }

  return st;
}

// Implementation of Retrieve
G4bool G4PersistencyManager::Retrieve(G4Event*& evt)
{
  if ( m_verbose > 2 ) {
    G4cout << "G4PersistencyManager::Retrieve(G4Event*&) is called."
           << G4endl;
  }

  if ( TransactionManager() == 0 ) return true;

  if ( f_pc->CurrentRetrieveMode("MCTruth") == false &&
       f_pc->CurrentRetrieveMode("Hits")    == false &&
       f_pc->CurrentRetrieveMode("Digits")  == false ) {
    return true;
  }

  // Call package dependent Initialize()
  //
  if ( ! f_is_initialized ) {
    f_is_initialized = true;
    if ( m_verbose > 1 ) {
      G4cout << "G4PersistencyManager:: Initializing Transaction ... "
             << G4endl;
    }
    Initialize();
  }

  // Start event IO transaction
  //
  if ( TransactionManager()->StartRead() ) {
    if ( m_verbose > 2 ) {
      G4cout << "G4PersistencyManager: Read transaction started."
             << G4endl;
    }
  } else {
    G4cerr << "TransactionManager::Retrieve(G4Event) - StartRead() failed."
           << G4endl;
    return false;
  }

  G4bool st = false;
  std::string file;

  // Retrieve a G4EVENT
  //
  std::string obj = "Hits";
  if ( f_pc->CurrentRetrieveMode(obj) == true ) {
    file = f_pc->CurrentReadFile(obj);
    if ( TransactionManager()->SelectReadFile(obj, file) ) {
      st = EventIO()->Retrieve(evt);
      if ( st && m_verbose > 1 ) {
        G4cout << " -- File : " << file << " -- Event# "
               << evt->GetEventID()
               << " -- G4Event is Retrieved." << G4endl;
      }
    } else {
      st = false;
    }
  }

  if ( st ) {
    TransactionManager()->Commit();
  } else {
    G4cerr << "G4PersistencyManager::Retrieve() - Transaction aborted."
           << G4endl;
    TransactionManager()->Abort();
  }

  return st;
}

// End of G4PersistencyManager.cc
