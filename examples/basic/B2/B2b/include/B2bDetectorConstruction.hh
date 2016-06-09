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
// $Id$
//
/// \file B2bDetectorConstruction.hh
/// \brief Definition of the B2bDetectorConstruction class

#ifndef B2bDetectorConstruction_h
#define B2bDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4UserLimits;

class B2bDetectorMessenger;
class B2MagneticField;

/// Detector construction class to define materials and geometry.
///
/// In addition a transverse uniform magnetic field is defined in
/// SetMagField() method which can be activated via a command
/// defined in the B2bDetectorMessenger class. 

class B2bDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    B2bDetectorConstruction();
    virtual ~B2bDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();

    // Set methods
    void SetMagField(G4double );
    void SetTargetMaterial (G4String );
    void SetChamberMaterial(G4String );
    void SetMaxStep (G4double );
    void SetCheckOverlaps(G4bool );

  private:
    // methods
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
  
    // data members
    G4LogicalVolume*   fLogicTarget;     // pointer to the logical Target
    G4LogicalVolume*   fLogicChamber;    // pointer to the logical Chamber

    G4Material*        fTargetMaterial;  // pointer to the target  material
    G4Material*        fChamberMaterial; // pointer to the chamber material

    G4UserLimits* fStepLimit;            // pointer to user step limits

    B2bDetectorMessenger*  fMessenger;   // messenger
    B2MagneticField*      fMagField;     // magnetic field
    
    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
