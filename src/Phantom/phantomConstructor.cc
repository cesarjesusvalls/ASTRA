#include "G4Step.hh"
#include "Randomize.hh"
#include "G4HCofThisEvent.hh"
#include "G4HCtable.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4UIcommand.hh"
#include "pCTRootPersistencyManager.hh"
#include "pCTEvent.hh"
#include "G4Timer.hh"
#include "stdio.h"
#include "G4PVPlacement.hh"
#include "phantomConstructor.hh"
#include "phantom.hh"
#include "G4NistManager.hh"
#include <G4VisAttributes.hh>
#include <TVector3.h>


phantomConstructor::phantomConstructor(){}

phantomConstructor::~phantomConstructor() {}


G4LogicalVolume *phantomConstructor::GetPhantom(void) {

//Getting the Elements
G4Element* O  = new G4Element("Oxygen"     , "O" , 8. , 15.9994*g/mole);
G4Element* H  = new G4Element("Hydrogen"   , "H" , 1. , 1.0079*g/mole);
G4Element* Bi = new G4Element("Bismuth"    , "Bi", 83., 208.9804*g/mole);
G4Element* C  = new G4Element("Carbon"     , "C" , 6. , 12.0107*g/mole);
G4Element* Cd = new G4Element("Cadmium"    , "Cd", 48., 112.4110*g/mole);
G4Element* Cl = new G4Element("Chlorine"   , "Cl", 17., 35.4530*g/mole);
G4Element* Co = new G4Element("Cobalt"     , "Co", 27., 58.933195*g/mole);
G4Element* Cr = new G4Element("Chromium"   , "Cr", 24., 51.9961*g/mole);
G4Element* Cu = new G4Element("Copper"     , "Cu", 29., 63.5463*g/mole);
G4Element* Fe = new G4Element("Iron"       , "Fe", 26., 55.8450*g/mole);
G4Element* K  = new G4Element("Potassium"  , "K" , 19., 39.0983*g/mole);
G4Element* Mn = new G4Element("Manganese"  , "Mn", 25., 54.9380*g/mole);
G4Element* Mo = new G4Element("Molybdenum" , "Mo", 42., 95.9620*g/mole);
G4Element* N  = new G4Element("Nitrogen"   , "N" , 7. , 14.0067*g/mole);
G4Element* Na = new G4Element("Sodium"     , "Na", 11., 22.9898*g/mole);
G4Element* Ni = new G4Element("Nickel"     , "Ni", 28., 58.6934*g/mole);
G4Element* P  = new G4Element("Phosphorus" , "P" , 15., 30.9738*g/mole);
G4Element* Pb = new G4Element("Lead"       , "Pb", 82., 207.21*g/mole);
G4Element* S  = new G4Element("Sulfur"     , "S" , 16., 32.0650*g/mole);
G4Element* Si = new G4Element("Silicon"    , "Si", 14., 28.0855*g/mole);
G4Element* Sn = new G4Element("Tin"        , "Sn", 50., 118.6900*g/mole);
G4Element* Zn = new G4Element("Zinc"       , "Zn", 30., 65.4094*g/mole);
G4Element* W  = new G4Element("Tungsten"   , "W",  74., 183.84*g/mole);
G4Element* Al = new G4Element("Aluminum"   , "Al", 13, 26.982*g/mole);
G4Element* Mg = new G4Element("Magnesium"  , "Mg", 12, 24.305*g/mole);
G4Element* Ca = new G4Element("Calcium", "Ca", 20, 40.078*g/mole);
G4Element* F = new G4Element("Fluorine", "F", 9, 18.998*g/mole);


// phantom materials from Leeds materials

                // Water equivalent material

                G4Material* WT1 = new G4Material("WT1", 1.00*g/cm3, 6);
                WT1->AddElement(H, 8.41*perCent);
                WT1->AddElement(C, 67.97*perCent);
                WT1->AddElement(N, 2.27*perCent);
                WT1->AddElement(O, 18.87*perCent);
                WT1->AddElement(Cl, 0.13*perCent);
                WT1->AddElement(Ca, 2.35*perCent);

                // Adipose

                G4Material* AP7 = new G4Material("AP7", 0.92*g/cm3, 6);
                AP7->AddElement(H, 8.36*perCent);
                AP7->AddElement(C, 69.14*perCent);
                AP7->AddElement(N, 2.36*perCent);
                AP7->AddElement(O, 16.93*perCent);
                AP7->AddElement(Cl, 0.14*perCent);
                AP7->AddElement(F, 3.07*perCent);

                // Lung is WT1 with different density
                G4NistManager *man = G4NistManager::Instance();
                 G4Material* LN10 = man->BuildMaterialWithNewDensity("LN10", "WT1", 0.3*g/cm3);
               
                // hard cortical bone
                G4Material* SB5 = new G4Material("SB5", 1.84*g/cm3, 6);
                SB5->AddElement(H, 2.60*perCent);
                SB5->AddElement(C, 30.58*perCent);
                SB5->AddElement(N, 0.98*perCent);
                SB5->AddElement(O, 38.93*perCent);
                SB5->AddElement(Cl, 0.06*perCent);
                SB5->AddElement(Ca, 26.85*perCent);

                // rib bone mad up of 46.5% hard bone and 53.5% WT1

                G4Material* RB2 = new G4Material("RB2", 1.40*g/cm3, 2);
                RB2->AddMaterial(WT1, 53.5*perCent);
                RB2->AddMaterial(SB5, 46.5*perCent);



    float phantomThickness = 0.1;
    float phantomSide = 5;
    float matSide = 1;
    G4bool checkOverlaps = true;

    G4ThreeVector pos1 = G4ThreeVector(-1*cm, 1*cm, 0);
    G4ThreeVector pos2 = G4ThreeVector(1*cm, 1*cm, 0);
    G4ThreeVector pos3 = G4ThreeVector(-1*cm, -1*cm, 0);
    G4ThreeVector pos4 = G4ThreeVector(1*cm, -1*cm, 0);



    G4Box* phantomShape =
    new G4Box("Phantom",
              0.5*phantomSide*cm,
              0.5*phantomSide*cm,
              phantomThickness*0.5*cm);

     G4LogicalVolume *phantomLogic
    = new G4LogicalVolume(phantomShape,
              WT1,
              "Phantom");


    G4Box* MUTShape = //shape of the Materials Under Test
    new G4Box("MUT",
          0.5*matSide*cm,
          0.5*matSide*cm,
          phantomThickness*0.5*cm);


    G4LogicalVolume *lungLogic = new G4LogicalVolume(MUTShape, LN10,"lung");  
    new G4PVPlacement(0, pos1, lungLogic, "lung", phantomLogic, false,0, checkOverlaps);

    G4LogicalVolume *ribBoneLogic = new G4LogicalVolume(MUTShape, RB2,"ribBone");  
    new G4PVPlacement(0, pos2, ribBoneLogic, "ribBone", phantomLogic, false,0, checkOverlaps);

    G4LogicalVolume *hcBoneLogic = new G4LogicalVolume(MUTShape, SB5,"hcBone");  
    new G4PVPlacement(0, pos3, hcBoneLogic, "hcBone", phantomLogic, false,0, checkOverlaps);

    G4LogicalVolume *adiposeLogic = new G4LogicalVolume(MUTShape, AP7,"adipose");  
    new G4PVPlacement(0, pos4, adiposeLogic, "adipose", phantomLogic, false,0, checkOverlaps);


    adiposeLogic->SetVisAttributes(G4Colour(0.3, 0.1, 0.6));
    hcBoneLogic->SetVisAttributes(G4Colour(0.2, 0.3, 0.2));
    lungLogic->SetVisAttributes(G4Colour(0.1, 0.2, 0.6));
    ribBoneLogic->SetVisAttributes(G4Colour(0.4, 0.2, 0.2));
    phantomLogic->SetVisAttributes(G4Colour(0.6, 0.2, 0.6));
    

return phantomLogic;

}
