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
/// \file examplepCT.cc
/// \brief Main program of the pCT example

#include "pCTDetectorConstruction.hh"
#include "pCTActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"
#include "time.h"

#include "pCTXML.hh"
#include "pCTRootPersistencyManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  
// new way to make Geant4 actually take a random seed!
  CLHEP::RanluxEngine defaultEngine(1234567, 4);
  G4Random::setTheEngine( &defaultEngine );
  G4int seed = time(NULL);
  G4Random::setTheSeed(seed);
  
  for(int arg=0; arg<argc; arg++)
    G4cout << "Argument " << arg << ":\t" << argv[arg] << G4endl;

  // Detect interactive mode (if no arguments) and define UI session
  //
  std::string rootfilename = "../output/simulation_file";
  G4String    xmlfilename  = "../config/pCT.xml";
  G4String    macroname    = "../mac/wrl_vis.mac";

  G4UIExecutive* ui = 0;
  if ( argc != 4 ) {
    ui = new G4UIExecutive(argc, argv);
  }
  else{
    rootfilename = argv[1];
    xmlfilename  = argv[2];
    macroname    = argv[3];
  }

#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Create ROOT the persistency manager.                                              
  pCTRootPersistencyManager* persistencyManager = pCTRootPersistencyManager::GetInstance();
    
   
  persistencyManager->Open(rootfilename); 
  if(persistencyManager->IsOpen()){
    G4cout << "The output ROOT file is open" << G4endl;
  }
  else{
    G4Exception("ExN02DetectorConstruction",
    "if(persistencyManager->IsOpen()",
    FatalException,
    "The file is not open");    
  }  

  persistencyManager->OpenXML(xmlfilename);
  pCTXML *pCTXMLInput = persistencyManager->GetXMLInput();

  // Set mandatory initialization classes
  //
  // Detector construction
  runManager->SetUserInitialization(new pCTDetectorConstruction());

  // Physics list
  G4VModularPhysicsList* physicsList = new QBBC;
  physicsList->SetVerboseLevel(0);
  runManager->SetUserInitialization(physicsList);
    
  // User action initialization
  runManager->SetUserInitialization(new pCTActionInitialization());
  
  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  if ( ! ui ) { 
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macroname);
  }
  else { 
    // interactive mode
    // UImanager->ApplyCommand("/control/execute ../mac/wrl_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  if(persistencyManager->IsOpen()){
    persistencyManager->Close(); 
  }

  delete persistencyManager;

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  
  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
