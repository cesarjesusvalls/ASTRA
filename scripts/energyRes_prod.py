import os

prog     = "../bin/pCTsim"
out_path = "/Users/cjesus/Dev/protonCT/output/simulation_file"
config   = "/Users/cjesus/Dev/protonCT/config/pCT.xml"
mac      = "../mac/run_vis.mac"

coating   = [0.05]#[0,0.01,0.02,0.1]
barZthick = [3,6,9] #[3,5,10] 
nlayers   = {3:120} #{3:120, 6:60, 9:40} 

for Z in barZthick:
	for C in coating:
		#command = prog + " " + out_path + "_" + str(Z) + "_" + str(C) + " " + config + " " + mac + " "
		command = prog + " " + out_path + "_" + str(Z) + "_" + str(C) + "_CMOS " + config + " " + mac + " "
		command = command + "--NOTusePhantom" + " " + "--useCMOS" + " " + "--barCoat" + " " + str(C) + " "
		command = command + "--barZ" + " " + str(Z) + " " + "--Nlay" + " " + str(nlayers[Z]) + " " + "--DoSquareSec" 
		print("executing: ", command)
		os.system(command)


