import os

prog     = "../bin/energy_res"
in_file  = "../output/material"
out_path = "../output/material"

Is_Interactive      = False
Cut_In_Straightness = True

Nprotons  = [1,2]
materials = [1,2,3,4,5]  

max_events = 1000000

commands = []

for M in materials:
    for P in Nprotons:
        command = prog
        command = command + " -z " + str(max_events)
        if Is_Interactive == False:
            command = command + " -b "
        command = command + " -i " + in_file  + str(M) + "_" + str(3) + "mm_" + str(P) + "protons_geant4.root"
        command = command + " -o " + out_path + str(M) + "_" + str(3) + "mm_" + str(P) + "protons_analysis.root"
        command = command + " -UsePreFitInfo"
        if Cut_In_Straightness == True:
            command = command + " -s 0.96 "
        commands.append(command)
        print("executing: ", command)
        os.system(command)
