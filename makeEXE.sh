#!/bin/bash 

#
# create build dir if does not exist, cd to build, conan install and then qmake
# 

mkdir -p build
cd build
rm -fr *.dmg *.app

# conan install
conan install .. --build missing
status=$?
if [[ $status != 0 ]]
then
    echo "EE-UQ: conan install failed";
    exit $status;
fi

# qmake
qmake ../EE-UQ.pro
status=$?
if [[ $status != 0 ]]
then
    echo "EE-UQ: qmake failed";
    exit $status;
fi

# make
make
status=$?;
if [[ $status != 0 ]]
then
    echo "EE-UQ: make failed";
    exit $status;
fi

if [[ "$OSTYPE" == "darwin"* ]]
then
    cp -r ../Examples ./EE_UQ.app/contents/MacOS
    rm -fr ./EE_UQ.app/contents/MacOS/Examples/.archive
    rm -fr ./EE_UQ.app/contents/MacOS/Examples/.aurore    
else
    #
    echo "No Examples Copied"
fi
