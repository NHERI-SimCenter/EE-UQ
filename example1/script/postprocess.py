#!/usr/bin/python

import sys
import re

EDPs = ['Node_4_Disp', 'Node_3_Disp']

inputArgs = sys.argv

with open ('node.out', 'rt') as inFile:
    line = inFile.readline()
    line = inFile.readline()
    line = inFile.readline()
    displ = line.split()
    numNode = len(displ)

inFile.close

#
# now process the input args and write the results file
#

outFile = open('results.out','w')

#
# note for now assuming no ERROR in user data
#

for i in EDPs[0:]:
    print i
    theList=i.split('_')

    if (theList[0] == 'Node'):
        nodeTag = int(theList[1])

        if (nodeTag > 0 and nodeTag <= numNode):
            if (theList[2] == 'Disp'):
                nodeDisp = displ[nodeTag-1]
                outFile.write(nodeDisp)
                outFile.write(' ')
            else:
                outFile.write('0. ')
        else:
            outFile.write('0. ')
    else:
        outFile.write('0. ')

outFile.close
