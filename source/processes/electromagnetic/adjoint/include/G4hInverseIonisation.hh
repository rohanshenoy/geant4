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
////////////////////////////////////////////////////////////////////////////////
//  Class:    G4hInverseIonisation
//  Author:         L. Desorgher
//  Organisation:   SpaceIT GmbH
//
//  Adjoint/reverse discrete ionisation for proton
////////////////////////////////////////////////////////////////////////////////

#ifndef G4hInverseIonisation_h
#define G4hInverseIonisation_h 1

#include "globals.hh"
#include "G4VAdjointReverseReaction.hh"

class G4AdjointhIonisationModel;

class G4hInverseIonisation : public G4VAdjointReverseReaction
{
 public:
  explicit G4hInverseIonisation(G4bool whichScatCase, const G4String& process_name,
                                G4AdjointhIonisationModel* aEmAdjointModel);
  ~G4hInverseIonisation() override = default;

  G4hInverseIonisation(G4hInverseIonisation&) = delete;
  G4hInverseIonisation& operator=(const G4hInverseIonisation& right) = delete;
};

#endif
