#This python script process the input and will use it to run SHA and ground motion selection
#In addition to providing the event file

import json
import os
import sys
import subprocess
import glob
import re

def computeScenario(gmConfig, location):
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    eqHazardPath = "{}/GMU/EQHazard.jar".format(scriptDir)
    simulateIMPath = "{}/GMU/SimulateIM".format(scriptDir)
    selectRecordPath = "{}/GMU/SelectRecord".format(scriptDir)
    recordDatabasePath = "{}/GMU/NGAWest2-1000.csv".format(scriptDir)

    #Separate Selection Config
    selectionConfig = gmConfig["RecordSelection"]
    del gmConfig["RecordSelection"]

    gmConfig["Site"] = {}
    gmConfig["Site"]["Type"] = "SingleLocation"
    gmConfig["Site"]["Location"] = {}
    gmConfig["Site"]["Location"]["Latitude"] = location[0]
    gmConfig["Site"]["Location"]["Longitude"] = location[1]

    #Adding the required output
    gmConfig["IntensityMeasure"]["EnableJsonOutput"] = True
    with open("./HazardWorkDir/Hazard_Scenario.json", 'w') as hazardFile:
        json.dump(gmConfig, hazardFile,  indent=4)
    
    #Now we need to run the EQHazard Process
    hazardCommand = ["java", "-jar", eqHazardPath, "./HazardWorkDir/Hazard_Scenario.json", "./HazardWorkDir/Hazard_Output.json"]
    hazardResult = subprocess.call(hazardCommand)

    if(hazardResult != 0):
        sys.stderr.write("Hazard analysis failed!")
        return -1

    #Now we need to run the SimulateIM Process
    #First we create a simulation config
    simConfig = {"GroundMotions": {"File": "./HazardWorkDir/Hazard_Output.json"}, "NumSimulations": 1, "SpatialCorrelation": True}

    with open("./HazardWorkDir/Sim_Config.json", 'w') as simConfigFile:
        json.dump(simConfig, simConfigFile,  indent=4)
    simulateCommand = [simulateIMPath, "./HazardWorkDir/Sim_Config.json", "./HazardWorkDir/Hazard_Sim.json"]
    simResult = subprocess.call(simulateCommand)

    if(simResult != 0):
        sys.stderr.write("Intensity measure simulation failed!")
        return -2
    
    #Now we can run record selection
    #
    selectionConfig["Target"]["File"] = "./HazardWorkDir/Hazard_Sim.json"
    selectionConfig["Database"]["File"] = recordDatabasePath
    with open("./HazardWorkDir/Selection_Config.json", 'w') as selectionConfigFile:
        json.dump(selectionConfig, selectionConfigFile,  indent=4)
    selectionCommand = [selectRecordPath, "./HazardWorkDir/Selection_Config.json", "./HazardWorkDir/Records_Selection.json"]
    simResult = subprocess.call(selectionCommand)

    if(simResult != 0):
        sys.stderr.write("Intensity measure simulation failed!")
        return -2


def readNGAWest2File(ngaW2FilePath, scaleFactor):
    series = []
    dt = 0.0
    with open(ngaW2FilePath, 'r') as recordFile:
        canRead = False #We need to process the header first
        for line in recordFile:
            if(canRead):
                series.extend([float(value) * scaleFactor * 9.81 for value in line.split()])

            elif("NPTS=" in line):
                dt = float(re.match(r"NPTS=.+, DT=\s+([0-9\.]+)\s+SEC", line).group(1))
                canRead = True
            

    return series, dt

def createNGAWest2Event(rsn, scaleFactor, recordsFolder, eventFilePath):
    pattern = os.path.join(recordsFolder, "RSN") + str(rsn)  +"_*.AT2"
    recordFiles = glob.glob(pattern)
    if(len(recordFiles) != 2):
        print 'Error finding NGA West 2 files.\n'\
        'Please download the files for record {} '\
        'from NGA West 2 website and place them in the records folder ({})'\
        .format(rsn, recordsFolder)
        exit(-1)

    h1, dt1 = readNGAWest2File(recordFiles[0], scaleFactor)
    h2, dt2 = readNGAWest2File(recordFiles[1], scaleFactor)

    patternH1 = {}
    patternH1["type"] = "UniformAcceleration"
    patternH1["timeSeries"] = "accel_X"
    patternH1["dof"] = 1

    patternH2 = {}
    patternH2["type"] = "UniformAcceleration"
    patternH2["timeSeries"] = "accel_Y"
    patternH2["dof"] = 2

    seriesH1 = {}
    seriesH1["name"] = "accel_X"
    seriesH1["type"] = "Value"
    seriesH1["dT"] = dt1
    seriesH1["data"] = h1

    seriesH2 = {}
    seriesH2["name"] = "accel_Y"
    seriesH2["type"] = "Value"
    seriesH2["dT"] = dt2
    seriesH2["data"] = h2

    event = {}
    event["name"] = "NGAW2_" + str(rsn)
    event["type"] = "Seismic" 
    event["description"] = "NGA West 2 record " + str(rsn) + " scaled by a factor of " + str(scaleFactor) 
    event["dT"] = dt1
    event["numSteps"] = len(h1)
    event["timeSeries"] = [seriesH1, seriesH2]
    event["pattern"] = [patternH1, patternH2]
    event["units"] = {
        "length": "m",
        "time": "sec"
    }

    eventsDict = {}
    eventsDict["Events"] = [event]
    eventsDict["RandomVariables"] = []

    with open(eventFilePath, 'w') as eventFile:
        json.dump(eventsDict, eventFile,  indent=4)


def main():
    inputArgs = sys.argv

    #Process only if --getRV is passed
    if not "--getRV" in inputArgs:
        sys.exit(0)

    #First let's process the arguments
    argBIM = inputArgs.index("--filenameBIM") + 1
    bimFilePath = inputArgs[argBIM]
    argEVENT = inputArgs.index("--filenameEVENT") + 1
    eventFilePath = inputArgs[argEVENT]

    #Ensure a hazard cache folder exist
    if not os.path.exists("./HazardWorkDir"):
        os.mkdir("./HazardWorkDir")

    with open(bimFilePath, 'r') as bimFile:
        bim = json.load(bimFile)
        location = [bim["GeneralInformation"]["location"]["latitude"], bim["GeneralInformation"]["location"]["longitude"]]
    
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    recordsFolder = "{}/GMU/NGAWest2Records".format(scriptDir)

    computeScenario(bim["Events"][0]["GroundMotion"], location)

    
    #We need to read the building location
   

    #Now we can start processing the event
    with open("./HazardWorkDir/Records_Selection.json", 'r') as selectionFile:
        recordSelection = json.load(selectionFile)

    selectedRecord = recordSelection["GroundMotions"][0]
    rsn = selectedRecord["Record"]["Id"]
    scaleFactor = selectedRecord["ScaleFactor"]
    
    createNGAWest2Event(rsn, scaleFactor, recordsFolder, eventFilePath)

if __name__== "__main__":
    main()
