import os

prog     = "../bin/pCTsim"
out_path = "../output/image_"
config   = "../config/pCT.xml"
mac      = "../mac/res_vis.mac"

Nprotons  = [1,2]
barZthick = [3] 
nlayers   = {3:120}

for Z in barZthick:
    for P in Nprotons:
        command = prog + " " + out_path + str(Z) + "mm_" + str(P) + "protons_geant4 " + config + " " + mac + " "
        command = command + "--NOTusePhantom" + " " + "--useCMOS" + " " + "--barCoat" + " " + str(0.05) + " "
        command = command + "--barZ" + " " + str(Z) + " " + "--Nlay" + " " + str(nlayers[Z]) + " " + "--DoSquareSec "
        command = command + "--NProt" + " " + str(P)
        command = command + "--NOTenergyWide " 
        print("executing: ", command)
        os.system(command)
