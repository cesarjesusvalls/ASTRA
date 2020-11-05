import os
import multiprocessing as mp

prog     = "../bin/pCTsim"
out_path = "/Users/cjesus/Dev/protonCT/output/dataset_Nprotons_"

Nprotons = range(0,2);  
commands = []

for P in Nprotons:
        command = prog + "-o " + out_path + str(P) + ".root"
        print("executing: ", command)
        os.system(command)

