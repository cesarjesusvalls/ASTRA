import os

prog     = "../bin/pCTsim"
out_path = "/Users/cjesus/Dev/protonCT/output/piposi_test_"
config   = "/Users/cjesus/Dev/protonCT/config/pCT.xml"
mac      = "../mac/run_vis.mac"

coating   = [0]
barZthick = [10]

for Z in barZthick:
	for C in coating:
		command = prog + " " + out_path + "_" + str(Z) + "_" + str(C) + " " + config + " " + mac + " "
		command = command + "--NOTusePhantom" + " " + "--NOTuseCMOS" + " " + "--barCoat" + " " + str(C) + " "
		command = command + "--barZ" + " " + str(Z) + " " + "--Nlay" + " " + str(1) + " " + "--DoSquareSec" 
		print("executing: ", command)
		os.system(command)
