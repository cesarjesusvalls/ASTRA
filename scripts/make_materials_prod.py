import os

prog     = "../bin/pCTsim"
out_path = "../output/material"
config   = "../config/pCT.xml"
mac      = "../mac/materials_vis.mac"

Nprotons  = [1]
materials = [1,2,3,4,5]  
nlayers   = {3:120}

for M in materials:
    for P in Nprotons:
        command = prog + " " + out_path + str(M) + "_" + str(3) + "mm_" + str(P) + "protons_geant4 " + config + " " + mac + " "
        command = command + "--useCMOS" + " " + "--barCoat" + " " + str(0.05) + " "
        command = command + "--barZ"  + " " + str(3) + " " + "--Nlay" + " " + str(nlayers[3]) + " " + "--DoSquareSec "
        command = command + "--NProt" + " " + str(P) + " "
        command = command + "--typeGUN M" + str(M) + " "
        command = command + "--NOTenergyWide "
        print("executing: ", command)
        os.system(command)
