# written: fmk, adamzs

# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange
    string_types = basestring
else:
    string_types = str

import json
import os
import subprocess
from time import gmtime, strftime
import posixpath

divider = '#' * 80
log_output = []

from WorkflowUtils import *

def main(run_type, inputFile, applicationsRegistry):
    # the whole workflow is wrapped within a 'try' block.
    # a number of exceptions (files missing, explicit application failures, etc.) are
    # handled explicitly to aid the user.
    # But unhandled exceptions case the workflow to stop with an error, handled in the
    # exception block way at the bottom of this main() function
    try:

        workflow_log(divider)
        workflow_log('Start of run')
        workflow_log(divider)
        workflow_log('workflow input file:       %s' % inputFile)
        workflow_log('application registry file: %s' % applicationsRegistry)
        workflow_log('runtype:                   %s' % run_type)
        workflow_log(divider)


        #
        # first we parse the applications registry to load all possible applications
        #  - for each application type we place in a dictionary key being name, value containing path to executable
        #
        with open(applicationsRegistry, 'r') as data_file:
            registryData = json.load(data_file)
            # convert all relative paths to full paths

        A = 'Applications'
        Applications = dict()
        appList = 'Event Modeling EDP Simulation UQ'.split(' ')
        appList = [a + A for a in appList]

        for app_type in appList:

            if app_type in registryData:
                xApplicationData = registryData[app_type]
                applicationsData = xApplicationData['Applications']

                for app in applicationsData:
                    appName = app['Name']
                    appExe = app['ExecutablePath']
                    if not app_type in Applications:
                        Applications[app_type] = dict()
                    Applications[app_type][appName] = appExe

        #
        # open input file, and parse json into data
        #

        with open(inputFile, 'r') as data_file:
            data = json.load(data_file)
            # convert all relative paths to full paths
            # relative2fullpath(data)

        if 'runDir' in data:
            runDIR = data['runDir']
        else:
            raise WorkFlowInputError('Need a runDir Entry')

        if 'remoteAppDir' in data:
            remoteAppDir = data['remoteAppDir']
        else:
            raise WorkFlowInputError('Need a remoteAppDir Entry')

        if 'localAppDir' in data:
            localAppDir = data['localAppDir']
        else:
            raise WorkFlowInputError('Need a localAppDir Entry')

        #
        # before running chdir to templatedir
        #

        workflow_log('run Directory:               %s' % runDIR)

        os.chdir(runDIR)
        os.chdir('templatedir')


        #
        # now we parse for the applications & app specific data in workflow
        #

        if 'Applications' in data:
            available_apps = data['Applications']
        else:
            raise WorkFlowInputError('Need an Applications Entry')

        #
        # get events, for each the  application and its data .. FOR NOW 1 EVENT
        #

        if 'Events' in available_apps:
            events = available_apps['Events']

            for event in events:

                if 'EventClassification' in event:
                    eventClassification = event['EventClassification']
                    if eventClassification == 'Earthquake' or eventClassification == 'Wind':
                        if 'Application' in event:
                            eventApplication = event['Application']
                            eventAppData = event['ApplicationData']
                            eventData = event['ApplicationData']

                            if eventApplication in Applications['EventApplications'].keys():
                                eventAppExe = Applications['EventApplications'].get(eventApplication)
                                workflow_log(remoteAppDir)
                                workflow_log(eventAppExe)
                                eventAppExeLocal = posixpath.join(localAppDir,eventAppExe)
                                eventAppExeRemote = posixpath.join(remoteAppDir,eventAppExe)
                                workflow_log(eventAppExeRemote)
                            else:
                                raise WorkFlowInputError('Event application %s not in registry' % eventApplication)

                        else:
                            raise WorkFlowInputError('Need an EventApplication section')


                    else:
                        raise WorkFlowInputError('Event classification must be Earthquake, not %s' % eventClassification)

                else:
                    raise WorkFlowInputError('Need Event Classification')

        else:
            raise WorkFlowInputError('Need an Events Entry in Applications')

        #
        # get modeling application and its data
        #

        if 'Modeling' in available_apps:
            modelingApp = available_apps['Modeling']

            if 'Application' in modelingApp:
                modelingApplication = modelingApp['Application']

                # check modeling app in registry, if so get full executable path
                modelingAppData = modelingApp['ApplicationData']
                if modelingApplication in Applications['ModelingApplications'].keys():
                    modelingAppExe = Applications['ModelingApplications'].get(modelingApplication)
                    modelingAppExeLocal = posixpath.join(localAppDir,modelingAppExe)
                    modelingAppExeRemote = posixpath.join(remoteAppDir,modelingAppExe)
                else:
                    raise WorkFlowInputError('Modeling application %s not in registry' % modelingApplication)

            else:
                raise WorkFlowInputError('Need a ModelingApplication in Modeling data')


        else:
            raise WorkFlowInputError('Need a Modeling Entry in Applications')

        #
        # get edp application and its data .. CURRENTLY MODELING APP MUST CREATE EDP
        #

        if 'EDP' in available_apps:
            edpApp = available_apps['EDP']
            
            if 'Application' in edpApp:
                edpApplication = edpApp['Application']
                
                # check modeling app in registry, if so get full executable path
                edpAppData = edpApp['ApplicationData']
                if edpApplication in Applications['EDPApplications'].keys():
                    edpAppExe = Applications['EDPApplications'].get(edpApplication)
                    edpAppExeLocal = posixpath.join(localAppDir,edpAppExe)
                    edpAppExeRemote = posixpath.join(remoteAppDir,edpAppExe)
                else:
                    raise WorkFlowInputError('EDP application {} not in registry'.format(edpApplication))
                
            else:
                raise WorkFlowInputError('Need an EDPApplication in EDP data')
            
        else:
            raise WorkFlowInputError('Need an EDP Entry in Applications')

        #
        # get simulation application and its data 
        #

        if 'Simulation' in available_apps:
            simulationApp = available_apps['Simulation']

            if 'Application' in simulationApp:
                simulationApplication = simulationApp['Application']

                # check modeling app in registry, if so get full executable path
                simAppData = simulationApp['ApplicationData']
                if simulationApplication in Applications['SimulationApplications'].keys():
                    simAppExe = Applications['SimulationApplications'].get(simulationApplication)
                    simAppExeLocal = posixpath.join(localAppDir,simAppExe)
                    simAppExeRemote = posixpath.join(remoteAppDir,simAppExe)
                else:
                    raise WorkFlowInputError('Simulation application {} not in registry'.format(simulationApplication))

            else:
                raise WorkFlowInputError('Need an SimulationApplication in Simulation data')


        else:
            raise WorkFlowInputError('Need a Simulation Entry in Applications')

        if 'UQ' in available_apps:
            uqApp = available_apps['UQ']

            if 'Application' in uqApp:
                uqApplication = uqApp['Application']

                # check modeling app in registry, if so get full executable path
                uqAppData = uqApp['ApplicationData']
                if uqApplication in Applications['UQApplications'].keys():
                    uqAppExe = Applications['UQApplications'].get(uqApplication)
                    uqAppExeLocal = posixpath.join(localAppDir,uqAppExe)
                    uqAppExeRemote = posixpath.join(localAppDir,uqAppExe)
                else:
                    raise WorkFlowInputError('UQ application {} not in registry'.format(uqApplication))

            else:
                raise WorkFlowInputError('Need a UQApplication in UQ data')


        else:
            raise WorkFlowInputError('Need a Simulation Entry in Applications')


        workflow_log('SUCCESS: Parsed workflow input')
        workflow_log(divider)

        #
        # now invoke the applications
        #

        # now we need to open buildingsfile and for each building
        #  - get RV for SAM file for building
        #  - get EDP for buildings and event
        #  - get SAM for buildings, event and EDP
        #  - perform Simulation
        #  - getDL

        bimFILE = 'dakota.json'
        eventFILE = 'EVENT.json'
        samFILE = 'SAM.json'
        edpFILE = 'EDP.json'
        simFILE = 'SIM.json'
        driverFile = 'driver'

        # open driver file & write building app (minus the --getRV) to it
        driverFILE = open(driverFile, 'w')

        # get RV for event
        eventAppDataList = ['"{}"'.format(eventAppExeRemote), '--filenameBIM', bimFILE, '--filenameEVENT', eventFILE]
        if (eventAppExe.endswith('.py')):
            eventAppDataList.insert(0, 'python')

        for key in eventAppData.keys():
            eventAppDataList.append(u"--" + key)
            value = eventAppData.get(key)
            eventAppDataList.append(u"" + value)
            
        for item in eventAppDataList:
            driverFILE.write('%s ' % item)
        driverFILE.write('\n')

        eventAppDataList.append('--getRV')
        if (eventAppExe.endswith('.py')):
            eventAppDataList[1] = u""+eventAppExeLocal
        else:
            eventAppDataList[0] = u""+eventAppExeLocal

        command, result, returncode = runApplication(eventAppDataList)
        log_output.append([command, result, returncode])

        # get RV for building model
        modelAppDataList = ['"{}"'.format(modelingAppExeRemote), '--filenameBIM', bimFILE, '--filenameEVENT', eventFILE, '--filenameSAM',
                            samFILE]

        if (modelingAppExe.endswith('.py')):
            modelAppDataList.insert(0, 'python')

        for key in modelingAppData.keys():
            modelAppDataList.append(u'--' + key)
            modelAppDataList.append(u'' + modelingAppData.get(key))

        for item in modelAppDataList:
            driverFILE.write('%s ' % item)
        driverFILE.write('\n')

        modelAppDataList.append('--getRV')

        if (modelingAppExe.endswith('.py')):
            modelAppDataList[1] = modelingAppExeLocal
        else:
            modelAppDataList[0] = modelingAppExeLocal

        command, result, returncode = runApplication(modelAppDataList)
        log_output.append([command, result, returncode])


        # get RV for EDP!
        edpAppDataList = ['"{}"'.format(edpAppExeRemote), '--filenameBIM', bimFILE, '--filenameEVENT', eventFILE, '--filenameSAM', samFILE,
                          '--filenameEDP', edpFILE]

        if (edpAppExe.endswith('.py')):
            edpAppDataList.insert(0, 'python')

        for key in edpAppData.keys():
            edpAppDataList.append(u'--' + key)
            edpAppDataList.append(u'' + edpAppData.get(key))

        for item in edpAppDataList:
            driverFILE.write('%s ' % item)
        driverFILE.write('\n')

        if (edpAppExe.endswith('.py')):
            edpAppDataList[1] = edpAppExeLocal
        else:
            edpAppDataList[0] = edpAppExeLocal

        edpAppDataList.append('--getRV')
        command, result, returncode = runApplication(edpAppDataList)
        log_output.append([command, result, returncode])

        # get RV for Simulation
        simAppDataList = ['"{}"'.format(simAppExeRemote), '--filenameBIM', bimFILE, '--filenameSAM', samFILE, '--filenameEVENT', eventFILE,
                          '--filenameEDP', edpFILE, '--filenameSIM', simFILE]

        if (simAppExe.endswith('.py')):
            simAppDataList.insert(0, 'python')

        for key in simAppData.keys():
            simAppDataList.append(u'--' + key)
            simAppDataList.append(u'' + simAppData.get(key))

        for item in simAppDataList:
            driverFILE.write('%s ' % item)
        driverFILE.write('\n')

        simAppDataList.append('--getRV')
        if (simAppExe.endswith('.py')):
            simAppDataList[1] = simAppExeLocal
        else:
            simAppDataList[0] = simAppExeLocal

        command, result, returncode = runApplication(simAppDataList)
        log_output.append([command, result, returncode])


        # perform the simulation
        driverFILE.close()

        uqAppDataList = ['"{}"'.format(uqAppExeLocal), '--filenameBIM', bimFILE, '--filenameSAM', samFILE, '--filenameEVENT', eventFILE,
                         '--filenameEDP', edpFILE, '--filenameSIM', simFILE, '--driverFile', driverFile]

        if (uqAppExe.endswith('.py')):
            uqAppDataList.insert(0, 'python')
            uqAppDataList[1] = uqAppExeLocal

        uqAppDataList.append(run_type)

        for key in uqAppData.keys():
            uqAppDataList.append(u'--' + key)
            value = uqAppData.get(key)            
            if isinstance(value, string_types):
                uqAppDataList.append(u'' + value)
            else:
                uqAppDataList.append(u'' + str(value))

        if run_type == 'run' or run_type == 'set_up':
            workflow_log('Running Simulation...')
            workflow_log(' '.join(uqAppDataList))
            command, result, returncode = runApplication(uqAppDataList)
            log_output.append([command, result, returncode])
            workflow_log('Simulation ended...')
        else:
            workflow_log('Setup run only. No simulation performed.')

    except WorkFlowInputError as e:
        print('workflow error: %s' % e.value)
        workflow_log('workflow error: %s' % e.value)
        workflow_log(divider)
        exit(1)

    # unhandled exceptions are handled here
    except Exception as e:
        print('workflow error: %s' % e.value)
        workflow_log('unhandled exception... exiting')
        raise

if __name__ == '__main__':

    if len(sys.argv) != 4:
        print('\nNeed three arguments, e.g.:\n')
        print('    python %s action workflowinputfile.json workflowapplications.json' % sys.argv[0])
        print('\nwhere: action is either check or run\n')
        exit(1)

    run_type = sys.argv[1]
    inputFile = sys.argv[2]
    applicationsRegistry = sys.argv[3]

    main(run_type, inputFile, applicationsRegistry)

    workflow_log_file = 'workflow-log-%s.txt' % (strftime('%Y-%m-%d-%H-%M-%S-utc', gmtime()))
    log_filehandle = open(workflow_log_file, 'w')

    print(type(log_filehandle))
    print(divider, file=log_filehandle)
    print('Start of Log', file=log_filehandle)
    print(divider, file=log_filehandle)
    print(workflow_log_file, file=log_filehandle)
    # nb: log_output is a global variable, defined at the top of this script.
    for result in log_output:
        print(divider, file=log_filehandle)
        print('command line:\n%s\n' % result[0], file=log_filehandle)
        print(divider, file=log_filehandle)
        print('output from process:\n%s\n' % result[1], file=log_filehandle)

    print(divider, file=log_filehandle)
    print('End of Log', file=log_filehandle)
    print(divider, file=log_filehandle)

    workflow_log('Log file: %s' % workflow_log_file)
    workflow_log('End of run.')

