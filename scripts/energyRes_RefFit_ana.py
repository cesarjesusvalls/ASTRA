import os
import multiprocessing as mp

prog     = "../bin/energyRes"
in_file  = "/Users/cjesus/Dev/protonCT/output/simulation_file_"
out_path = "/Users/cjesus/Dev/protonCT/output/energyRes_"

measurements = [2]  
coating      = [0.05]
barZthick    = [3]

commands = []

command = prog + " -z 1000000 -i /Users/cjesus/Dev/protonCT/output/reference_file_3_0.05_CMOS.root -m 2"
command = command + "-o /Users/cjesus/Dev/protonCT/output/erase_this.root" # + " -s " + str(0.96) 
commands.append(command)
print("executing: ", command)
os.system(command)
