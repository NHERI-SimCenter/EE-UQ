#!/bin/bash 

release=${1:-"NO_RELEASE"}

#
# create build dir if does not exist, cd to build, conan install and then qmake
# 

mkdir -p build
cd build
rm -fr *.dmg *.app

# conan install
conan install .. --output-folder=. --build=missing

if [[ $0 != 0 ]]; then
    echo "EE-UQ: conan install failed";
fi

#
# qmake
#

if [ -n "$release" ] && [ "$release" = "release" ]; then
    echo "******** RELEASE BUILD *************"    
    qmake QMAKE_CXXFLAGS+=-D_SC_RELEASE QMAKE_CXXFLAGS+=-D_INCLUDE_USER_PASS ../EE-UQ.pro
    cmd_status=$?; if [[ $cmd_status != 0 ]]; then echo "EE-UQ: qmake failed"; exit $cmd_status; fi        
else
    echo "********* NON RELEASE BUILD ********"
    qmake ../EE-UQ.pro QMAKE_CXXFLAGS+=-D_INCLUDE_USER_PASS
    cmd_status=$?; if [[ $cmd_status != 0 ]]; then echo "EE-UQ: qmake failed"; exit $cmd_status; fi    
fi





#
# make
#

touch ../main.cpp
make -j 4


cmd_status=$?;
if [[ $cmd_status != 0 ]]
then
    echo "EE-UQ: make failed";
    exit $cmd_status;
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
