import os

prog     = "../bin/pCTsim"
out_path = "/Users/cjesus/Dev/protonCT/output/simulation_file"
config   = "/Users/cjesus/Dev/protonCT/config/pCT.xml"
mac      = "../mac/run_vis.mac"

coating   = [0,0.01,0.02,0.1]
barZthick = [3,5,10]  

for Z in barZthick:
	for C in coating:
		command = prog + " " + out_path + "_" + str(Z) + "_" + str(C) + " " + config + " " + mac + " "
		command = command + "--NOTusePhantom" + " " + "--NOTuseCMOS" + " " + "--barCoat" + " " + str(C) + " "
		command = command + "--barZ" + " " + str(Z) + " " + "--Nlay" + " " + str(int(400/(Z))) + " " + "--DoSquareSec" 
		print("executing: ", command)
		os.system(command)
