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
// $Id: G4CollisionNNElastic.hh,v 1.2 2006-06-29 20:32:58 gunter Exp $ //

#ifndef G4CollisionNNElastic_h
#define G4CollisionNNElastic_h

#include "globals.hh"
#include "G4VCollision.hh"
#include "G4VCrossSectionSource.hh"
#include "G4VAngularDistribution.hh"
#include "G4KineticTrackVector.hh"
#include "G4VElasticCollision.hh"
#include <vector>

class G4KineticTrack;

class G4CollisionNNElastic : public G4VElasticCollision
{

public:

  G4CollisionNNElastic();

  virtual ~G4CollisionNNElastic();

  G4bool operator==(const G4CollisionNNElastic &right) const;
  G4bool operator!=(const G4CollisionNNElastic &right) const;

  virtual G4bool IsInCharge(const G4KineticTrack& trk1, 
			    const G4KineticTrack& trk2) const;

  virtual G4String GetName() const { return "NN Elastic Collision"; }
  

protected:

  virtual const G4VCrossSectionSource* GetCrossSectionSource() const { return crossSectionSource; }
  virtual const G4VAngularDistribution* GetAngularDistribution() const { return angularDistribution; }

  virtual const std::vector<G4String>& GetListOfColliders(G4int whichOne) const;  


private:  

  G4VCrossSectionSource* crossSectionSource;
  G4VAngularDistribution* angularDistribution;

  std::vector<G4String> colliders1;
  std::vector<G4String> colliders2;

};

#endif
