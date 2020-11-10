import os
import multiprocessing as mp

prog     = "../bin/energyRes"
in_file  = "/Users/cjesus/Dev/protonCT/output/simulation_file_"
out_path = "/Users/cjesus/Dev/protonCT/output/energyRes_"

measurements = [2]  
coating      = [0.05]#[0,0.01,0.02,0.1]
barZthick    = [3]#[3,6,9]  

commands = []

for M in measurements:
    for Z in barZthick:
        for C in coating:
            command = prog + " -z 1000 -i " + in_file + str(Z) + "_" + str(C) + "_CMOS.root "
            command = command + "-o /Users/cjesus/Dev/protonCT/output/energyRes_" + str(M) + "_" + str(Z) + "_" + str(C) + "_CMOS.root" + " -m " + str(M) 
            commands.append(command)
            print("executing: ", command)
            os.system(command)

# def run_command(command):
#     print("executing: ", command)
#     os.system(command)

# processes = []
# for command in commands:
#     p = mp.Process(target=run_command,args=[command])
#     p.start()
#     processes.append(p)

# for process in processes:
#     process.join()


