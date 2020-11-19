import os

prog     = "../bin/energy_res"
in_file  = "../output/resolution_"
out_path = "../output/resolution_"

Is_Interactive      = False
Cut_In_Straightness = True

Nprotons  = [1]
barZthick = [3] 

max_events = 1000000

commands = []

for Z in barZthick:
    for P in Nprotons:
        command = prog
        command = command + " -z " + str(max_events)
        if Is_Interactive == False:
            command = command + " -b "
        command = command + " -i " + in_file  + str(Z) + "mm_" + str(P) + "protons_geant4.root"
        command = command + " -o " + out_path + str(Z) + "mm_" + str(P) + "protons_analysis.root"
        if Cut_In_Straightness == True:
            command = command + " -s 0.96 "
        commands.append(command)
        print("executing: ", command)
        os.system(command)
