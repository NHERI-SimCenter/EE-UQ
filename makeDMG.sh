# remove & rebuild app and macdeploy it

appName="EE-UQ"
appFile="EE_UQ.app"
dmgFile="EE-UQ_Mac_Download.dmg"
APP_NAME="EE_UQ"
DMG_FILE_NAME="${APP_NAME}-MacDownload.dmg"
VOLUME_NAME="${APP_NAME}"
SOURCE_FOLDER_PATH="app"

QTDIR="/Users/fmckenna/Qt/5.15.2/clang_64/"

pathToBackendApps="/Users/fmckenna/NHERI/SimCenterBackendApplications"
pathToOpenSees="/Users/fmckenna/bin/OpenSees3.2.2"
pathToDakota="/Users/fmckenna/dakota-6.12.0"

#pathToPython="/Users/fmckenna/PythonEnvR2D"


#
# build it
#

./makeEXE.sh
cd build

#
# Check to see if the app built
#

if ! [ -x "$(command -v open $pathApp)" ]; then
	echo "$appFile did not build. Exiting."
	exit 
fi

#
# macdeployqt it
#

macdeployqt ./EE_UQ.app -qmldir=$HOME/NHERI/QS3hark

#
# add missing files from macdeployqt (a known bug)
#


mkdir -p ./$appFile/Contents/plugins/renderers/
cp -R $QTDIR/plugins/renderers/libopenglrenderer.dylib ./$appFile/Contents/plugins/renderers/

echo "cp -R $QTDIR/plugins/renderplugins/libscene2d.dylib ./$appFile/Contents/plugins/renderplugins/"

mkdir -p ./$appFile/Contents/plugins/renderplugins/
cp -R $QTDIR/plugins/renderplugins/libscene2d.dylib ./$appFile/Contents/plugins/renderplugins/


# copy applications folderm opensees and dakota
echo "cp -fR $pathToBackendApps/applications ./$appFile/Contents/MacOS"
cp -fR $pathToBackendApps/applications ./$appFile/Contents/MacOS
mkdir  ./$appFile/Contents/MacOS/applications/opensees
mkdir  ./$appFile/Contents/MacOS/applications/dakota
echo "cp -fr $pathToOpenSees/* $pathApp/Contents/MacOS/applications/opensees"
cp -fr $pathToOpenSees/* ./$appFile/Contents/MacOS/applications/opensees
cp -fr $pathToDakota/*  ./$appFile/Contents/MacOS/applications/dakota

cp /usr/local/opt/libomp/lib/libomp.dylib ./$appFile/Contents/MacOS/applications/performUQ/SimCenterUQ
install_name_tool -change /usr/local/opt/libomp/lib/libomp.dylib @executable_path/libomp.dylib ./$appFile/Contents/MacOS/applications/performUQ/SimCenterUQ/nataf_gsa


# clean up
declare -a notWantedApp=("createBIM" 
			 "performRegionalMapping"
			 "performRegionalEventSimulation"
			 "performDL"			
			 "createEDP/standardEarthquakeEDP_R"
			 "createEDP/userEDP_R"
			 "createEDP/gmtEDP"			 
			 "createEDP/simpleEDP"
			 "createEDP/standardWindEDP"
			 "createEVENT/GeoClawOpenFOAM"			 
			 "createEVENT/ASCE7_WindSpeed"
			 "createEVENT/CFDEvent"
			 "createEVENT/hazardBasedEvent"
			 "createEVENT/DEDM_HRP"
			 "createEVENT/stochasticWind"
			 "createEVENT/stochasticWind"
			 "createEVENT/ASCE7_WindSpeed"
			 "createEVENT/LLNL_SW4"
			 "createEVENT/windTunnelExperiment"
			 "createEVENT/LowRiseTPU"
			 "createEVENT/pointWindSpeed"
			 "createSAM/MDOF-LU"
			 "performSIMULATION/IMasEDP"
			 "performSIMULATION/extractPGA"
			 "performSIMULATION/openSees_R"
			)

for app in "${notWantedApp[@]}"
do
   echo "removing $app"
   rm -fr ./$appFile/Contents/MacOS/applications/$app
done

find ./$appFile -name __pycache__ -exec rm -rf {} +;

#
# load my credential file


userID="../userID.sh"

if [ ! -f "$userID" ]; then

    echo "creating dmg $dmgFile"
    rm $dmgFile
    hdiutil create $dmgFile -fs HFS+ -srcfolder ./$appFile -format UDZO -volname $appName

    echo "No password & credential file to continue with codesig and App store verification"
    exit
fi

source $userID
echo $appleID

#codesign
echo "codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $appFile"
codesign --deep --force --verbose --options=runtime  --sign "$appleCredential" $appFile

# create dmg
#echo "hdiutil create $dmgFile -fs HFS+ -srcfolder ./$appFile -format UDZO -volname $appName"
#hdiutil create $dmgFile -fs HFS+ -srcfolder ./$appFile -format UDZO -volname $appName
mkdir app
mv $appFile ./app

create-dmg \
  --volname "${VOLUME_NAME}" \
  --background "../backgroundMacInstall.png" \
  --window-pos 200 120 \
  --window-size 550 400 \
  --icon-size 150 \
  --icon "${APP_NAME}.app" 150 190 \
  --hide-extension "${APP_NAME}.app" \
  --app-drop-link 400 185 \
  "${DMG_FILE_NAME}" \
  "${SOURCE_FOLDER_PATH}"

mv ./app/$appFile ./
rm -fr app

#codesign dmg
echo "codesign --force --sign "$appleCredential" $dmgFile"
codesign --force --sign "$appleCredential" $dmgFile

echo "Issue the following: " 
echo "xcrun altool --notarize-app -u $appleID -p $appleAppPassword -f ./$dmgFile --primary-bundle-id altool"
echo ""
echo "returns id: ID .. wait for email indicating success"
echo "To check status"
echo "xcrun altool --notarization-info ID  -u $appleID  -p $appleAppPassword"
echo ""
echo "Finally staple the dmg"
echo "xcrun stapler staple \"$appName\" $dmgFile"
