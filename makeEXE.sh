#!/bin/bash 

#
# create build dir if does not exist, cd to build, conan install and then qmake
# 

mkdir -p build
cd build
rm -fr *.dmg *.app

# conan install
conan install .. --build missing

if [[ $0 != 0 ]];
then
    echo "EE-UQ: conan install failed";
fi

#
# qmake
#

if [ -n "$release" ] && [ "$release" = "release" ]; then
    echo "******** RELEASE BUILD *************"    
    qmake QMAKE_CXXFLAGS+="-D_SC_RELEASE" ../EE-UQ.pro
    status=$?; if [[ $status != 0 ]]; then echo "EE-UQ: qmake failed"; exit $status; fi        
else
    echo "********* NON RELEASE BUILD ********"
    qmake ../EE-UQ.pro
    status=$?; if [[ $status != 0 ]]; then echo "EE-UQ: qmake failed"; exit $status; fi    
fi





#
# make
#

touch ../WorkflowAppEE-UQ.cpp
make -j 4


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
