import os

prog     = "../bin/pCTsim"
out_path = "/Users/cjesus/Dev/protonCT/output/reference_file"
config   = "/Users/cjesus/Dev/protonCT/config/pCT.xml"
mac      = "../mac/run_vis.mac"

coating   = [0.05]
barZthick = [3] 
nlayers   = {3:120} 

for Z in barZthick:
    for C in coating:
        command = prog + " " + out_path + "_" + str(Z) + "_" + str(C) + "_CMOS " + config + " " + mac + " "
        command = command + "--NOTusePhantom" + " " + "--useCMOS" + " " + "--barCoat" + " " + str(C) + " "
        command = command + "--barZ" + " " + str(Z) + " " + "--Nlay" + " " + str(nlayers[Z]) + " " + "--DoSquareSec" 
        print("executing: ", command)
        os.system(command)
