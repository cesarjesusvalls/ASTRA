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
/// \file pCTPrimaryGeneratorAction.cc
/// \brief Implementation of the pCTPrimaryGeneratorAction class

#include "pCTPrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "pCTRootPersistencyManager.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "time.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTPrimaryGeneratorAction::pCTPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0), 
  fEnvelopeBox(0)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="gamma");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(6.*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTPrimaryGeneratorAction::~pCTPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void pCTPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{  
    G4double envSizeXY = 0;
    G4double envSizeZ = 0;

    if (!fEnvelopeBox){
        G4LogicalVolume* envLV  = G4LogicalVolumeStore::GetInstance()->GetVolume("Envelope");
        if ( envLV ) fEnvelopeBox = dynamic_cast<G4Box*>(envLV->GetSolid());
    }

    if ( fEnvelopeBox ) {
        envSizeXY = fEnvelopeBox->GetXHalfLength()*2.;
        envSizeZ  = fEnvelopeBox->GetZHalfLength()*2.;
    }

    else  {
        G4ExceptionDescription msg;
        msg << "Envelope volume of box shape not found.\n"; 
        msg << "Perhaps you have changed geometry.\n";
        msg << "The gun will be place at the center.";
        G4Exception("pCTPrimaryGeneratorAction::GeneratePrimaries()",
        "MyCode0002",JustWarning,msg);
    }

    pCTRootPersistencyManager *InputPersistencyManager = pCTRootPersistencyManager::GetInstance();
    pCTXMLInput = InputPersistencyManager->GetXMLInput();

    // G4double particlerandom = 3*G4UniformRand();
    // G4String particlename;
    // if(particlerandom < 1)                       particlename = "proton";
    // if(particlerandom >=1 && particlerandom <2)  particlename = "pi+";
    // if(particlerandom >=2 && particlerandom <3)  particlename = "e+";
 
    // fParticleGun->SetParticleMomentum(800*CLHEP::MeV);
    // G4ParticleDefinition* particleDefinition 
    // = G4ParticleTable::GetParticleTable()->FindParticle(particlename);
    // fParticleGun->SetParticleDefinition(particleDefinition);

    // G4double x0 =   (G4UniformRand()-0.5)*pCTXMLInput->GetPlaneColumns()*0.04;
    // G4double y0 =   (G4UniformRand()-0.5)*pCTXMLInput->GetPlaneColumns()*0.036;
    // G4double z0 =   pCTXMLInput->GetPosZ0()*10-5;
    // std::cout << "z0: " << z0 << std::endl;
    // fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

    double iniEnergy = -999;

    for (int ind=0; ind<pCTXMLInput->GetNProtons(); ind++){
        if(pCTXMLInput->GetBeamType() == "Rectangular"){
            G4double x0 =   (G4UniformRand()-0.5)*pCTXMLInput->GetPlaneColumns()*0.04;
            G4double y0 =   (G4UniformRand()-0.5)*pCTXMLInput->GetPlaneColumns()*0.036;
            G4double z0 =   pCTXMLInput->GetPosZ0()*10-5;
            fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
        }
        iniEnergy = pCTXMLInput->GetIniPGUNEnergy();
        if(pCTXMLInput->UseEnergyWide()){
            iniEnergy = 36 + (G4UniformRand()*194.);
        }
    }

    fParticleGun->SetParticleEnergy(iniEnergy);
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
