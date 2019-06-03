# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange

#else:
#    from past.builtins import basestring

import os
import platform
import shutil
import subprocess
import stat
from preprocessJSON import preProcessDakota

#First we need to set the path and environment
home = os.path.expanduser('~')
env = os.environ
if os.getenv("PEGASUS_WF_UUID") is not None:
    print("Pegasus job detected - Pegasus will set up the env")
elif platform.system() == 'Darwin':
    env["PATH"] = env["PATH"] + ':{}/bin'.format(home)
    env["PATH"] = env["PATH"] + ':{}/dakota/bin'.format(home)
elif platform.system() == 'Linux':
    env["PATH"] = env["PATH"] + ':{}/bin'.format(home)
    env["PATH"] = env["PATH"] + ':{}/dakota/dakota-6.5/bin'.format(home)
else:
    print("PLATFORM {} NOT RECOGNIZED".format(platform.system))

#Reading input arguments
bimName = sys.argv[2]
samName = sys.argv[4]
evtName = sys.argv[6]
edpName = sys.argv[8]
simName = sys.argv[10]
driverFile = sys.argv[12]

runDakota = sys.argv[13]

#Run Preprocess for Dakota
scriptDir = os.path.dirname(os.path.realpath(__file__))
# preProcessArgs = ["python", "{}/preprocessJSON.py".format(scriptDir), bimName, evtName,\
# samName, edpName, lossName, simName, driverFile, scriptDir, bldgName]
# subprocess.call(preProcessArgs)
numRVs = preProcessDakota(bimName, evtName, samName, edpName, simName, driverFile)

#Setting Workflow Driver Name
workflowDriverName = 'workflow_driver'
if platform.system() == 'Windows':
    workflowDriverName = 'workflow_driver.bat'

#Create Template Directory and copy files
templateDir = "templatedir"
#if os.path.exists(templateDir):
#    shutil.rmtree(templateDir)

#os.mkdir(templateDir)
st = os.stat(workflowDriverName)
os.chmod(workflowDriverName, st.st_mode | stat.S_IEXEC)
shutil.copy(workflowDriverName, templateDir)
shutil.copy("{}/dpreproSimCenter".format(scriptDir), os.getcwd())
shutil.copy(bimName, "bim.j")
shutil.copy(evtName, "evt.j")
exists = os.path.isfile(samName)
if exists:
    shutil.copy(samName, "sam.j")

shutil.copy(edpName, "edp.j")

exists = os.path.isfile(simName)
if exists:
    shutil.copy(simName, "sim.j")

shutil.copy("dakota.in", "../")
os.chdir("../")

if runDakota == "run":

    dakotaCommand = "dakota -input dakota.in -output dakota.out -error dakota.err"
    subprocess.Popen(dakotaCommand, shell=True).wait()

    #Postprocess Dakota results
    #postprocessCommand = '{}/postprocessDAKOTA {} {} {} {} dakotaTab.out'.format(
    #    scriptDir, numRVs, numSamples, bimName, edpName)

    #subprocess.Popen(postprocessCommand, shell=True).wait()
