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
/// \file pCTEventAction.cc
/// \brief Implementation of the pCTEventAction class

#include "pCTEventAction.hh"
#include "pCTRunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

#include "pCTRootPersistencyManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTEventAction::pCTEventAction(pCTRunAction* runAction)
  : G4UserEventAction()//,  fRunAction(runAction)//,
  //  fEdep(0.) 
{} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTEventAction::~pCTEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void pCTEventAction::BeginOfEventAction(const G4Event*)
{    
  
    fEdep[0] = 0.;
    fEdep[1] = 0.;
    fEdep[2] = 0.;
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void pCTEventAction::EndOfEventAction(const G4Event*)
{   
    std::cout<< "Energy in the first Detector "<<fEdep[0]<<" Energy in the second Detector "<<fEdep[1]<<" Energy in the third Detector "<<fEdep[2]<<"\n";
   // accumulate statistics in run action
  //     fRunAction->AddEdep(fEdep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
