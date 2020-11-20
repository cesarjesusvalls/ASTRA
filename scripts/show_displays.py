import os

prog     = "../bin/event_display"
in_file  = "../output/simulation_file_"
out_path = "../output/erase_me.root"

Nprotons  = [4]
barZthick = [3] 

max_events = 1000000

for Z in barZthick:
    for P in Nprotons:
        command = prog
        command = command + " -z " + str(max_events)
        command = command + " -i " + in_file  + str(Z) + "mm_" + str(P) + "protons_geant4.root"
        command = command + " -o " + out_path
        command = command + " -showRECO "
        print("executing: ", command)
        os.system(command)
