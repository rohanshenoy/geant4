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
//
// $Id: G4QuasiElasticChannel.cc,v 1.9 2010-09-17 11:34:29 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

// Author : Gunter Folger March 2007
// Modified by Mikhail Kossov. Apr2009, E/M conservation: ResidualNucleus is added (ResNuc)
// Class Description
// Final state production model for theoretical models of hadron inelastic
// quasi elastic scattering in geant4;
// Class Description - End
//
// Modified:
// 20110805  M. Kelsey -- Follow change to G4V3DNucleus::GetNucleons()
// 20110808  M. Kelsey -- Move #includes from .hh, add many missing

#include "G4QuasiElasticChannel.hh"

#include "G4Fancy3DNucleus.hh"
#include "G4DynamicParticle.hh"
#include "G4HadTmpUtil.hh"	  /* lrint */
#include "G4KineticTrack.hh"
#include "G4KineticTrackVector.hh"
#include "G4LorentzVector.hh"
#include "G4Neutron.hh"
#include "G4Nucleon.hh"
#include "G4Nucleus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4QuasiElRatios.hh"
#include "globals.hh"
#include <vector>

//#define debug_scatter


G4QuasiElasticChannel::G4QuasiElasticChannel()
  : theQuasiElastic(G4QuasiElRatios::GetPointer()),
    the3DNucleus(new G4Fancy3DNucleus) {}

G4QuasiElasticChannel::~G4QuasiElasticChannel()
{
  delete the3DNucleus;
}

G4double G4QuasiElasticChannel::GetFraction(G4Nucleus &theNucleus,
    const G4DynamicParticle & thePrimary)
{
    #ifdef debug_scatter   
      G4cout << "G4QuasiElasticChannel:: P=" << thePrimary.GetTotalMomentum()
             << ", pPDG=" << thePrimary.GetDefinition()->GetPDGEncoding()
             << ", Z = "  << theNucleus.GetZ_asInt())
             << ", N = "  << theNucleus.GetN_asInt())
             << ", A = "  << theNucleus.GetA_asInt() << G4endl;
    #endif

  std::pair<G4double,G4double> ratios;
  ratios=theQuasiElastic->GetRatios(thePrimary.GetTotalMomentum(),
                                    thePrimary.GetDefinition()->GetPDGEncoding(),
                                    theNucleus.GetZ_asInt(),
                                    theNucleus.GetN_asInt());
    #ifdef debug_scatter   
      G4cout << "G4QuasiElasticChannel::ratios " << ratios.first << " x " <<ratios.second
             << "  = " << ratios.first*ratios.second << G4endl;
    #endif
        
  return ratios.first*ratios.second;
}

G4KineticTrackVector * G4QuasiElasticChannel::Scatter(G4Nucleus &theNucleus,
                                                      const G4DynamicParticle & thePrimary)
{
  G4int A=theNucleus.GetA_asInt();
  G4int Z=theNucleus.GetZ_asInt();
  //   build Nucleus and choose random nucleon to scatter with
  the3DNucleus->Init(theNucleus.GetA_asInt(),theNucleus.GetZ_asInt());
  const std::vector<G4Nucleon>& nucleons=the3DNucleus->GetNucleons();
  G4double targetNucleusMass=the3DNucleus->GetMass();                  // M.K. ResNuc
  G4LorentzVector targetNucleus4Mom(0.,0.,0.,targetNucleusMass);      // M.K. ResNuc
  G4int index;
  do
  {
    index=G4lrint((A-1)*G4UniformRand());
  } while (index < 0 || index >= static_cast<G4int>(nucleons.size()));
  G4ParticleDefinition * pDef= nucleons[index].GetDefinition();

  G4int resA=A-1;                                                     // M.K. ResNuc
  G4int resZ=Z-static_cast<int>(pDef->GetPDGCharge());                // M.K. ResNuc
  G4ParticleDefinition* resDef=G4Neutron::Neutron(); // Resolve t-p=nn problem M.K. ResNuc
  G4double residualNucleusMass=resDef->GetPDGMass();                  // M.K. ResNuc
  if(resZ)
  {
    resDef=G4ParticleTable::GetParticleTable()->FindIon(resZ,resA,0,resZ);// M.K. ResNuc
    residualNucleusMass=resDef->GetPDGMass();                         // M.K. ResNuc
  }
  else {
    residualNucleusMass*=resA;
  }
   #ifdef debug_scatter
     G4cout<<"G4QElChan::Scatter: neutron - proton? A ="<<A<<", Z="<<Z<<", projName="
           <<pDef->GetParticleName()<<G4endl;
   #endif

  G4LorentzVector pNucleon=nucleons[index].Get4Momentum();
  G4double residualNucleusEnergy=std::sqrt(residualNucleusMass*residualNucleusMass+
                                           pNucleon.vect().mag2());   // M.K. ResNuc
  pNucleon.setE(targetNucleusMass-residualNucleusEnergy);             // M.K. ResNuc
  G4LorentzVector residualNucleus4Mom=targetNucleus4Mom-pNucleon;     // M.K. ResNuc
 
  std::pair<G4LorentzVector,G4LorentzVector> result;
 
  result=theQuasiElastic->Scatter(pDef->GetPDGEncoding(),pNucleon,
                                  thePrimary.GetDefinition()->GetPDGEncoding(),
  thePrimary.Get4Momentum());
  G4LorentzVector scatteredHadron4Mom=result.second;                  // M.K. ResNuc
  if (result.first.e() <= 0.)
  {
    //G4cout << "Warning - G4QuasiElasticChannel::Scatter no scattering" << G4endl;
    //return 0;       //no scatter
    G4LorentzVector scatteredHadron4Mom=thePrimary.Get4Momentum();   // M.K. ResNuc
    residualNucleus4Mom=G4LorentzVector(0.,0.,0.,targetNucleusMass); // M.K. ResNuc
    resDef=G4ParticleTable::GetParticleTable()->FindIon(Z,A,0,Z);    // M.K. ResNuc
  }

#ifdef debug_scatter
  G4LorentzVector EpConservation=pNucleon+thePrimary.Get4Momentum() 
                                 - result.first - result.second;
  if (   (EpConservation.vect().mag2() > .01*MeV*MeV )
      || (std::abs(EpConservation.e()) > 0.1 * MeV ) ) 
  {
    G4cout << "Warning - G4QuasiElasticChannel::Scatter E-p non conservation : "
           << EpConservation << G4endl;
  }    
#endif

  G4KineticTrackVector * ktv;
  ktv=new G4KineticTrackVector();
  G4KineticTrack * sPrim=new G4KineticTrack(thePrimary.GetDefinition(),
                                            0.,G4ThreeVector(0), scatteredHadron4Mom);
  ktv->push_back(sPrim);
  if (result.first.e() > 0.)
  {
    G4KineticTrack * sNuc=new G4KineticTrack(pDef, 0.,G4ThreeVector(0), result.first);
    ktv->push_back(sNuc);
  }
  if(resZ || resA==1) // For the only neutron or for tnuclei with Z>0    M.K. ResNuc
  {
    G4KineticTrack * rNuc=new G4KineticTrack(resDef,
                           0.,G4ThreeVector(0), residualNucleus4Mom); // M.K. ResNuc
    ktv->push_back(rNuc);                                             // M.K. ResNuc
  }
  else // The residual nucleus consists of only neutrons                 M.K. ResNuc
  {
    residualNucleus4Mom/=resA;     // Split 4-mom of A*n system equally  M.K. ResNuc
    for(G4int in=0; in<resA; in++) // Loop over neutrons in A*n system.  M.K. ResNuc
    {
      G4KineticTrack* rNuc=new G4KineticTrack(resDef,
                           0.,G4ThreeVector(0), residualNucleus4Mom); // M.K. ResNuc
      ktv->push_back(rNuc);                                           // M.K. ResNuc
    }
  }
#ifdef debug_scatter
  G4cout<<"G4QElC::Scat: Nucleon: "<<result.first <<" mass "<<result.first.mag() << G4endl;
  G4cout<<"G4QElC::Scat: Project: "<<result.second<<" mass "<<result.second.mag()<< G4endl;
#endif
  return ktv;
}
