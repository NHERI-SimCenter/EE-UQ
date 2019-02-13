import sys
from shutil import copyfile    

def main():
    inputArgs = sys.argv

    #First let's process the arguments
    argBIM = inputArgs.index("--filenameBIM") + 1
    bimFile = inputArgs[argBIM]
    argEVENT = inputArgs.index("--filenameEVENT") + 1
    eventFile = inputArgs[argEVENT]
    argInputFile = inputArgs.index("--fileName") + 1
    inputFile = inputArgs[argInputFile]

    # only copy file if --getRV, which occurs when argc == 10
    argc = len(sys.argv)
    if (argc == 10):
        if (inputFile != eventFile):
            copyfile(inputFile, eventFile)
            print("Copied File: %s to %s\n",inputFile, eventFile)
        else:
            print("FIle not copied: %s to %s\n",inputFile, eventFile)

if __name__== "__main__":
    main()
