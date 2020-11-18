import os
import multiprocessing as mp

prog     = "../bin/energyRes"
in_file  = "/Users/cjesus/Dev/protonCT/output/"
out_path = "/Users/cjesus/Dev/protonCT/output/dist_"

materials = [1,2,3,4,5]  

commands = []

for M in materials:
    command = prog + " -b -z 10000 -i " + in_file + "M" + str(M) + "_3_0.05_CMOS.root " + " -s " + str(0)
    command = command + " -o " + out_path + "M" + str(M) + ".root"
    commands.append(command)
    print("executing: ", command)
    os.system(command)

# command = prog + " -b -z 1000000 -i /Users/cjesus/Dev/protonCT/output/reference_for_fit_3_0.05_CMOS.root " + " -s " + str(0)
# command = command + " -o " + "erase_this.root"
# commands.append(command)
# print("executing: ", command)
# os.system(command)
