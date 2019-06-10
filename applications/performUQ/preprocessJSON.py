# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange

#else:
#    from past.builtins import basestring

import json
import os
import sys
import platform
import posixpath

numRandomVariables = 0

numNormalUncertain = 0
normalUncertainName=[]
normalUncertainMean =[]
normalUncertainStdDev =[]

numLognormalUncertain = 0;
lognormalUncertainName=[];
lognormalUncertainMean =[];
lognormalUncertainStdDev =[];

numUniformUncertain = 0;
uniformUncertainName=[];
uniformUncertainLower =[];
uniformUncertainUpper =[];

numContinuousDesign = 0;
continuousDesignName=[];
continuousDesignLower =[];
continuousDesignUpper =[];
continuousDesignInitialPoint =[];

numConstantState = 0;
constantStateName=[];
constantStateValue =[];

numWeibullUncertain = 0;
weibullUncertainName=[];
weibullUncertainAlphas =[];
weibullUncertainBetas =[];

numGammaUncertain = 0;
gammaUncertainName=[];
gammaUncertainAlphas =[];
gammaUncertainBetas =[];

numGumbellUncertain = 0;
gumbellUncertainName=[];
gumbellUncertainAlphas =[];
gumbellUncertainBetas =[];

numBetaUncertain = 0;
betaUncertainName=[];
betaUncertainLower =[];
betaUncertainHigher =[];
betaUncertainAlphas =[];

numDiscreteDesignSetString = 0
discreteDesignSetStringName=[]
discreteDesignSetStringValues =[]

numResultFiles = 0
outputResultFiles = []

def preProcessDakota(bimName, evtName, samName, edpName, simName, driverFile):

    global numRandomVariables
    global numNormalUncertain
    global normalUncertainName
    global normalUncertainMean
    global normalUncertainStdDev

    global numDiscreteDesignSetString
    global discreteDesignSetStringName
    global discreteDesignSetStringValues

    global outputResultFiles
    global numResultFiles

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
    # get result files
    #

    if 'ResultFiles' in data:
        numResultFiles = 1;
        outputResultFiles.append("EVENT.json");
    else:
        numResultFiles = 0;

    # 
    # parse the data
    #

    bimExists = parseFileForRV(bimName)
    evtExists = parseFileForRV(evtName)
    samExists = parseFileForRV(samName)
    simExists = parseFileForRV(simName)
    edpExists = parseFileForRV(edpName)

    #Setting Workflow Driver Name
    workflowDriverName = 'workflow_driver'
    remoteWorkflowDriverName = 'workflow_driver'
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

    if (numLognormalUncertain > 0):
        f.write('lognormal_uncertain = ' '{}'.format(numLognormalUncertain))
        f.write('\n')
        f.write('means = ')
        for i in range(numLognormalUncertain):
            f.write('{}'.format(lognormalUncertainMean[i]))
            f.write(' ')
        f.write('\n')

        f.write('std_deviations = ')
        for i in range(numLognormalUncertain):
            f.write('{}'.format(lognormalUncertainStdDev[i]))
            f.write(' ')
        f.write('\n')
    
        f.write('descriptors = ')    
        for i in range(numLognormalUncertain):
            f.write('\'')
            f.write(lognormalUncertainName[i])
            f.write('\' ')
        f.write('\n')

    if (numUniformUncertain > 0):
        f.write('uniform_uncertain = ' '{}'.format(numUniformUncertain))
        f.write('\n')
        f.write('lower_bounds = ')
        for i in range(numUniformUncertain):
            f.write('{}'.format(uniformUncertainLower[i]))
            f.write(' ')
        f.write('\n')
                
        f.write('upper_bounds = ')
        for i in range(numUniformUncertain):
            f.write('{}'.format(uniformUncertainUpper[i]))
            f.write(' ')
        f.write('\n')
    
        f.write('descriptors = ')    
        for i in range(numUniformUncertain):
            f.write('\'')
            f.write(uniformUncertainName[i])
            f.write('\' ')
        f.write('\n')


    if (numContinuousDesign > 0):
        f.write('continuous_design = ' '{}'.format(numContinuousDesign))
        f.write('\n')
        
        f.write('initial_point = ')
        for i in range(numContinuousDesign):
            f.write('{}'.format(continuousDesignInitialPoint[i]))
            f.write(' ')
        f.write('\n')

        f.write('lower_bounds = ')
        for i in range(numContinuousDesign):
            f.write('{}'.format(continuousDesignLower[i]))
            f.write(' ')
        f.write('\n')

        f.write('upper_bounds = ')
        for i in range(numContinuousDesign):
            f.write('{}'.format(continuousDesignUpper[i]))
            f.write(' ')
        f.write('\n')
    
        f.write('descriptors = ')    
        for i in range(numContinuousDesign):
            f.write('\'')
            f.write(continuousDesignName[i])
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
    localDir = data["localAppDir"];

    if (runType == "local"):
        numCPUs = 4
        f.write("fork asynchronous evaluation_concurrency = %d\n" % numCPUs)
        f.write("analysis_driver = '{}'\n".format(workflowDriverName))
    else:
        f.write('fork asynchronous\n')
        f.write("analysis_driver = '{}'\n".format(remoteWorkflowDriverName))
    
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
            known = False
            if(edp["type"] == "max_abs_acceleration"):
                edpAcronym = "PFA"
                floor = edp["floor"]
                known = True

            elif(edp["type"] == "max_drift"):
                edpAcronym = "PID"
                floor = edp["floor2"]
                known = True

            elif(edp["type"] == "max_pressure"):
                edpAcronym = "PSP"
                floor = edp["floor2"]
                known = True

            elif(edp["type"] == "max_rel_disp"):
                edpAcronym = "PFD"
                floor = edp["floor"]
                known = True

            else:
                f.write("'{}' ".format(edp["type"]))

            if (known == True):
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
    if bimExists == True: f.write('perl dpreproSimCenter params.in bim.j ' + bimName + '\n')
    if samExists == True: f.write('perl dpreproSimCenter params.in sam.j ' + samName + '\n')
    if evtExists == True: f.write('perl dpreproSimCenter params.in evt.j ' + evtName + '\n')
    if edpExists == True: f.write('perl dpreproSimCenter params.in edp.j ' + edpName + '\n')

    scriptDir = os.path.dirname(os.path.realpath(__file__))

    with open(driverFile) as fp:
        for line in fp:
            print(line)
            print(localDir)
            line = line.replace(localDir,remoteDir)
            f.write(line)
            print(line)

    f.write('#comment to fix a bug\n')
    files = " "
    files =  files + "".join([str(i) for i in outputResultFiles])
    numR = str(numResultFiles)
    if (runType == "local"):
        f.write('"{}'.format(scriptDir) + '/extractEDP" ' + edpName + ' results.out ' + bimName + ' ' + numR + ' ' + files + '\n')

    else:
        extractEDPCommand = posixpath.join(remoteDir, 'applications/performUQ/extractEDP')
        f.write(extractEDPCommand + ' ' + edpName + ' results.out ' + bimName + ' ' + numR + ' ' + files + '\n')

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

    global numLognormalUncertain
    global lognormalUncertainName
    global lognormalUncertainMean
    global lognormalUncertainStdDev

    global numUniformUncertain
    global uniformUncertainName
    global uniformUncertainLower
    global uniformUncertainUpper

    global numContinuousDesign
    global continuousDesignName
    global continuousDesignLower
    global continuousDesignUpper
    global continuousDesignInitialPoint

    global numConstantState
    global constantStateName
    global constantStateValue

    global numWeibullUncertain
    global weibullUncertainName
    global weibullUncertainAlphas
    global weibullUncertainBetas

    global numGammaUncertain
    global gammaUncertainName
    global gammaUncertainAlphas
    global gammaUncertainBetas

    global numGumbellUncertain
    global gumbellUncertainName
    global gumbellUncertainAlphas
    global gumbellUncertainBetas

    global numBetaUncertain
    global betaUncertainName
    global betaUncertainLower
    global betaUncertainHigher
    global betaUncertainAlphas

    global numDiscreteDesignSetString
    global normalDiscreteDesignSetName
    global normalDiscreteSetValues


    global numResultFiles
    global outputResultFiles

    exists = os.path.isfile(fileName)

    print(exists)

    if not exists:
        return False

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

                elif (k["distribution"] == "Lognormal"):
                    lognormalUncertainName.append(k["name"])
                    lognormalUncertainMean.append(k["mean"])
                    lognormalUncertainStdDev.append(k["stdDev"])
                    numLognormalUncertain += 1
                elif (k["distribution"] == "Constant"):
                    constantStateName.append(k["name"])
                    constantStateValue.append(k["value"])
                    numConstantState += 1
                elif (k["distribution"] == "Uniform"):
                    print("Hellooo,, Setting lower upper bounds...")
                    uniformUncertainName.append(k["name"])
                    uniformUncertainLower.append(k["lowerbound"])
                    uniformUncertainUpper.append(k["upperbound"])
                    numUniformUncertain += 1
                elif (k["distribution"] == "ContinuousDesign"):
                    continuousDesignName.append(k["name"])
                    continuousDesignLower.append(k["lowerbound"])
                    continuousDesignUpper.append(k["upperbound"])
                    continuousDesignInitialPoint.append(k["initialpoint"])
                    numContinuousDesign += 1
                elif (k["distribution"] == "Weibull"):
                    weibullUncertainName.append(k["name"])
                    weibullUncertainAlphas.append(k["scaleparam"])
                    weibullUncertainBetas.append(k["shapeparam"])
                    numWeibullUncertain += 1
                elif (k["distribution"] == "Gamma"):
                    gammaUncertainName.append(k["name"])
                    gammaUncertainAlphas.append(k["alphas"])
                    gammaUncertainBetas.append(k["betas"])
                    numGammaUncertain += 1
                elif (k["distribution"] == "Gumbell"):
                    gumbellUncertainName.append(k["name"])
                    gumbellUncertainAlphas.append(k["alphas"])
                    gumbellUncertainBetas.append(k["betas"])
                    numGumbellUncertain += 1
                elif (k["distribution"] == "Beta"):
                    betaUncertainName.append(k["name"])
                    betaUncertainLower.append(k["upperBounds"])
                    betaUncertainUpper.append(k["lowerBounds"])
                    betaUncertainAlphas.append(k["alphas"])
                    betaUncertainBetas.append(k["betas"])
                    numBetaUncertain += 1
                elif (k["distribution"] == "discrete_design_set_string"):
                    discreteDesignSetStringName.append(k["name"])
                    elements =[];
                    for l in k["elements"]:
                        elements.append(l)
                    elements.sort()
                    discreteDesignSetStringValues.append(elements)
                    print(elements)
                    numDiscreteDesignSetString += 1
                    numRandomVariables += 1

        if data.get("resultFiles"):
            for k in data["resultFiles"]:
                outputResultFiles.append(k)
                numResultFiles += 1
                print(k)

    return True
