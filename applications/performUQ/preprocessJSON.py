# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange
else:
    from past.builtins import basestring

import json
import os
import sys
import platform

numRandomVariables = 0
numNormalUncertain = 0
normalUncertainName=[]
normalUncertainMean =[]
normalUncertainStdDev =[]

numDiscreteDesignSetString = 0
discreteDesignSetStringName=[]
discreteDesignSetStringValues =[]

def preProcessDakota(bimName, evtName, samName, edpName, simName, driverFile):

    global numRandomVariables
    global numNormalUncertain
    global normalUncertainName
    global normalUncertainMean
    global normalUncertainStdDev

    global numDiscreteDesignSetString
    global discreteDesignSetStringName
    global discreteDesignSetStringValues

    #
    # get UQ method data
    #

    with open('dakota.json') as data_file:    
        data = json.load(data_file)
        
    uqData = data["UQ_Method"];
    samplingData = uqData["samplingMethodData"];
    method = samplingData["method"];
    if (method == "Monte Carlo"):
        method = 'random'
    else:
        method = 'lhs'
    numSamples=samplingData["samples"];
    seed = samplingData["seed"];

    # 
    # parse the data
    #

    parseFileForRV(bimName)
    parseFileForRV(evtName)
    parseFileForRV(samName)
    parseFileForRV(simName)
    parseFileForRV(edpName)

    #Setting Workflow Driver Name
    workflowDriverName = 'workflow_driver'
    if platform.system() == 'Windows':
        workflowDriverName = 'workflow_driver.bat'

    #
    # Write the input file: dakota.in 
    #

    # write out the method data
    f = open('dakota.in', 'w')

    f.write("environment\n")
    f.write("tabular_data\n")
    f.write("tabular_data_file = \'dakotaTab.out\'\n\n")

    f.write('method,\n')
    f.write('sampling\n');
    method = samplingData["method"];
    if (method == "Monte Carlo"):
        method = 'random'
    else:
        method = 'lhs'
    numSamples=samplingData["samples"];
    seed = samplingData["seed"];
    f.write('sample_type = ' '{}'.format(method))
    f.write('\n');
    f.write('samples = ' '{}'.format(numSamples))
    f.write('\n');
    f.write('seed = ' '{}'.format(seed))
    f.write('\n\n')

    # write out the variable data
    f.write('variables,\n')
    f.write('active uncertain \n')

    if (numNormalUncertain > 0):
        f.write('normal_uncertain = ' '{}'.format(numNormalUncertain))
        f.write('\n')
        f.write('means = ')
        for i in range(numNormalUncertain):
            f.write('{}'.format(normalUncertainMean[i]))
            f.write(' ')
        f.write('\n')

        f.write('std_deviations = ')
        for i in range(numNormalUncertain):
            f.write('{}'.format(normalUncertainStdDev[i]))
            f.write(' ')
        f.write('\n')

        f.write('descriptors = ')    
        for i in range(numNormalUncertain):
            f.write('\'')
            f.write(normalUncertainName[i])
            f.write('\' ')
        f.write('\n')

    if (numDiscreteDesignSetString > 0):
        f.write('discrete_uncertain_set\n')
        f.write('string ' '{}'.format(numDiscreteDesignSetString))
        f.write('\n')

        f.write('num_set_values = ')    
        for i in range(numDiscreteDesignSetString):
            #f.write('\'')
            numElements = len(discreteDesignSetStringValues[i])
            f.write(' ' '{}'.format(numElements))
            #f.write(length(discreteDesignSetStringValues[i]))
            print(discreteDesignSetStringValues[i])
            print(numElements)
            #f.write('\' ')

        f.write('\n')
        f.write('set_values  ')    
        for i in range(numDiscreteDesignSetString):
            elements = discreteDesignSetStringValues[i]
            for j in elements:
                f.write('\'' '{}'.format(j))
                f.write('\' ')
            f.write('\n')

        f.write('descriptors = ')    
        for i in range(numDiscreteDesignSetString):
            f.write('\'')
            f.write(discreteDesignSetStringName[i])
            f.write('\' ')

        f.write('\n')

    f.write('\n\n')

    # write out the interface data
    f.write('interface,\n')
    runType = data["runType"];
    remoteDir = data["remoteAppDir"];

    if (runType == "local"):
        numCPUs = 4
        f.write("fork asynchronous evaluation_concurrency = %d\n" % numCPUs)
    else:
        f.write('fork asynchronous\n')

    f.write("analysis_driver = '{}'\n".format(workflowDriverName))
    f.write('parameters_file = \'params.in\' \n')
    f.write('results_file = \'results.out\' \n')
    f.write('work_directory directory_tag \n')
    f.write('copy_files = \'templatedir/*\' \n')
    f.write('named \'workdir\' file_save  directory_save \n')
    f.write('aprepro \n')
    f.write('\n')

    #
    # write out the responses
    #

    with open(edpName) as data_file:    
        data = json.load(data_file)

    numResponses=data["total_number_edp"]


    f.write('responses, \n')
    f.write('response_functions = ' '{}'.format(numResponses))
    f.write('\n')
    f.write('response_descriptors = ')    

    
    for event in data["EngineeringDemandParameters"]:
        eventIndex = data["EngineeringDemandParameters"].index(event)
        for edp in event["responses"]:
            if(edp["type"] == "max_abs_acceleration"):
                edpAcronym = "PFA"
                floor = edp["floor"]

            elif(edp["type"] == "max_drift"):
                edpAcronym = "PID"
                floor = edp["floor1"]

            else:
                edpAcronym = "UnknownEDP"

            for dof in edp["dofs"]:
                f.write("'{}-{}-{}-{}' ".format(eventIndex + 1, edpAcronym, floor, dof))


    f.write('\n')
    f.write('no_gradients\n')
    f.write('no_hessians\n\n')
    f.close()  # you can omit in most cases as the destructor will call it

    #
    # Write the workflow driver
    #

    f = open(workflowDriverName, 'w')

    # want to dprepro the files with the random variables
    f.write('perl dpreproSimCenter params.in bim.j ' + bimName + '\n')
    f.write('perl dpreproSimCenter params.in sam.j ' + samName + '\n')
    f.write('perl dpreproSimCenter params.in evt.j ' + evtName + '\n')
    f.write('perl dpreproSimCenter params.in edp.j ' + edpName + '\n')

    scriptDir = os.path.dirname(os.path.realpath(__file__))

    with open(driverFile) as fp:
        for line in fp:
            f.write(line)
            print(line)

    f.write('\n')
    if (runType == "local"):
        f.write(scriptDir + '/extractEDP ' + edpName + ' results.out \n')
    else:
        f.write(remoteDir + '/applications/performUQ/extractEDP ' + edpName + ' results.out \n')

    # Run 
    #f.write('rm -f *.com *.done *.dat *.log *.sta *.msg')
    #f.write('echo 1 >> results.out\n')
    f.close()

    return numRandomVariables

def parseFileForRV(fileName):
    global numRandomVariables
    global numNormalUncertain
    global normalUncertainName
    global normalUncertainMean
    global normalUncertainStdDev

    global numDiscreteDesignSetString
    global normalDiscreteDesignSetName
    global normalDiscreteSetValues

    print(fileName)

    with open(fileName,'r') as data_file:
        data = json.load(data_file)
        if data.get("randomVariables"):
            for k in data["randomVariables"]:
                if (k["distribution"] == "Normal"):
                    normalUncertainName.append(k["name"])
                    normalUncertainMean.append(k["mean"])
                    normalUncertainStdDev.append(k["stdDev"])
                    numNormalUncertain += 1
                    numRandomVariables += 1

                if (k["distribution"] == "discrete_design_set_string"):
                    print(k)
                    discreteDesignSetStringName.append(k["name"])
                    elements =[];
                    for l in k["elements"]:
                        elements.append(l)
                    elements.sort()
                    discreteDesignSetStringValues.append(elements)
                    print(elements)
                    numDiscreteDesignSetString += 1
                    numRandomVariables += 1


