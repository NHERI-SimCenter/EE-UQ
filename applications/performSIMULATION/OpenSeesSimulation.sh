#!/bin/bash

#echo $#

if [ "$#" -lt 10 ]; then
    echo $#
    echo "wrong # parameters, want OpenSeesSimulation.sh --filenameBIM BIM.json --filenameSAM SAM.json --filenameEVENT EVENT.json --filenameEDP EDP.json --filenameSIM SIM.json <--getRV>
    echo "wrong #paramateres, want: performSimulation fileBIM fileSAM fileEVENT fileEDP"
    exit
fi

# set filenames
filenameBIM="$2"
filenameSAM="$4"
filenameEVENT="$6"
filenameEDP="$8"
filenameSIM="${10}"

scriptDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$#" -eq 10 ]; then
# perform simulation
$scriptDIR/OpenSeesPreprocessor $2 $4 $6 $8 ${10} example.tcl
OpenSees example.tcl
$scriptDIR/OpenSeesPostprocessor $2 $4 $6 $8
# rm example.tcl

else

# get random variables
$scriptDIR/OpenSeesPreprocessor $2 $4 $6 ${10} 

fi
