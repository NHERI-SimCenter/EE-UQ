#!/bin/bash
set -euo pipefail

# Usage: makeMac.sh [--arch <x86_64 or arm64>] [--release] [--wipe]
# Defaults: arch = ""

PROJECT_NAME="EE_UQ"

CONAN_PROFILE="default"
BUILD_DIR="build"
ARCH=""
PREFIX="$(pwd)/DEPS"
RELEASE="NO"
RELEASE_FLAG="-DNO_RELEASE"
APPLICATIONS_DIR=$(pwd)/../SimCenterBackendApplications/applications
PYTHON_DIR="${HOME}/cpython"
WIPE="FALSE"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --arch|-a)
            ARCH="$2"
            if [[ "$ARCH" != "x86_64" && "$ARCH" != "arm64" ]]; then
                echo "Error: Invalid architecture '$ARCH'."
                echo "Supported architectures: x86_64, arm64"
                exit 1
            fi

            CONAN_PROFILE="macos-$ARCH"
            BUILD_DIR="build_$ARCH"
            PREFIX="${PREFIX}_$ARCH"
            APPLICATIONS_DIR="${APPLICATIONS_DIR}_$ARCH"	    

            if [[ "$ARCH" == "x86_64" ]]; then
		pathToOpenSees="${HOME}/bin/OpenSeesLatest_x86_64"
		pathToDakota="${HOME}/dakota/dakota-6.24.0-x86"
		PYTHON_DIR="${PYTHON_DIR}/python_x86_64"		
	    else
		pathToOpenSees="${HOME}/bin/OpenSeesLatest_arm64"
		pathToDakota="${HOME}/dakota/dakota-6.24.0"
		PYTHON_DIR="${PYTHON_DIR}/python_arm64"				
            fi	    
	    
            shift 2
            ;;
	--release|-r)
	    RELEASE="YES"
	    RELEASE_FLAG="-D_SC_RELEASE"	    
	    shift 1
	    ;;
	--wipe|-w)
	    WIPE="TRUE"
	    shift 1
	    ;;
        *)	    
            echo "Unknown argument: $1"
            echo "Usage: $0 --arch <arm64|x86_64>"
            exit 1
            ;;
    esac
done


echo "Architecture: ${ARCH}"
echo "Conan profile : ${CONAN_PROFILE}"
echo "Build directory: ${BUILD_DIR}"
echo ""

# Define some helpers to print messages
die() {
    echo ""    
    echo "$1"
    echo ""    
    exit 1
}

msg() {
    echo ""
    echo "$1"
    echo ""
}

#
# Check for Conan .. 
#

if ! command -v conan &> /dev/null; then
    die "FAIL: 'conan' command not found. Please install it or un from python_env."
fi

#
# Build dependencies first
#

#if [ ! -d "$PREFIX" ]; then
#    msg "--- making DEPS in $PREFIX ---"
#    if [ -n "${ARCH}" ]; then
#        bash makeMacDeps.sh --arch "${ARCH}"
#    else
#        bash makeMacDeps.sh
#    fi
#else
#    msg "--- DEPS ALREADY BUILT in $PREFIX ---"
#fi

#
# Remove old build dir
#

if [[ "$WIPE" == "TRUE" ]]; then
    if [[ -n "$BUILD_DIR" && "$BUILD_DIR" != "$HOME" ]]; then
	echo "Cleaning up old build directory..."
	rm -rf "${BUILD_DIR}"
    fi
fi

#
# Conan build dependencies
#

msg "Running conan install..."
 
conan install . --output-folder="${BUILD_DIR}" --build missing -s build_type=Release -pr "${CONAN_PROFILE}" || die "FAIL : Conan install failed."

#
# Configure CMake
#

CMAKE_ARCH_FLAG=""
if [ -n "${ARCH}" ]; then
    CMAKE_ARCH_FLAG="-DCMAKE_OSX_ARCHITECTURES=${ARCH}"
fi

msg "Configuring CMake ..."
set -x
cmake -B "${BUILD_DIR}" -S . \
    -DCMAKE_TOOLCHAIN_FILE="${BUILD_DIR}/conan_toolchain.cmake" \
    -DCMAKE_PREFIX_PATH="${PREFIX}" ${CMAKE_ARCH_FLAG} -DCMAKE_CXX_FLAGS=${RELEASE_FLAG} \
    -DCMAKE_BUILD_TYPE=Release || die "FAIL: CMake configure failed."

set +x

#
# in case no wipe, removing app and touchingh WorkflowApp and main
#

touch WorkflowAppEE-UQ.cpp main.cpp
rm -fr "${BUILD_DIR}/${PROJECT_NAME}.app"

#
# Now build With CMake
#

msg "Building with 8 cores..."
cmake --build "${BUILD_DIR}" --parallel 8 || die "FAIL: CMake build failed."

APP_NAME="${PROJECT_NAME}.app"
APP_DIR="${BUILD_DIR}/${APP_NAME}"

#
# Copy Examples folder
#

cp -fr Examples "${APP_DIR}/Contents/MacOS/"
rm -fr "${APP_DIR}/Contents/MacOS/Examples/.archive"
rm -fr "${APP_DIR}/Contents/MacOS/Examples/.aurore"
rm -fr "${APP_DIR}/Contents/MacOS/Examples/.gitignore"

if [[ "$RELEASE" != "YES" ]]; then

    # Copy backend, don't bother cleanup unwanted as if arch specifdied, cannot set backend to it    
    mkdir  -p "${APP_DIR}/Contents/MacOS/applications"
    cp -fr "${APPLICATIONS_DIR}"/* "$APP_DIR/Contents/MacOS/applications"    
    
    die "Build complete!"
fi

# ===============================
# Release Build
# ===============================

msg "Now Doing Stuff for a Release"

#
# macdeploy the application
#

msg "macdeployqt ${APP_DIR}"
macdeployqt "${APP_DIR}"

#
# Copy needed files (backend, opensees,dakota)
#

msg "copying Shark, Backend, OpenSees and Dakota to bundle"

cp -fr "$HOME/NHERI/QS3hark/resources" "${APP_DIR}/Contents/MacOS"

mkdir  "${APP_DIR}/Contents/MacOS/applications"
mkdir  "${APP_DIR}/Contents/MacOS/applications/opensees"
mkdir  "${APP_DIR}/Contents/MacOS/applications/dakota"

cp -fr "${APPLICATIONS_DIR}"/* "$APP_DIR/Contents/MacOS/applications"
cp -fr "${pathToOpenSees}"/* "./${APP_DIR}/Contents/MacOS/applications/opensees"
cp -fr "${pathToDakota}"/*   "./${APP_DIR}/Contents/MacOS/applications/dakota"

#
# remove unwanted applications from backend
#

UNWANTED_FILE="unwanted_apps.txt"

# Check if file exists
if [[ ! -f "$UNWANTED_FILE" ]]; then
    echo "Error: $UNWANTED_FILE not found!"
    exit 1
fi

# Read file line by line and remove each app
echo "Removing unwanted: "
while IFS= read -r app || [[ -n "$app" ]]; do
    # Skip empty lines and comments
    [[ -z "$app" || "$app" =~ ^# ]] && continue
    
    echo "Removing $app: ./$APP_DIR/Contents/MacOS/applications/$app"
    rm -fr "./$APP_DIR/Contents/MacOS/applications/$app"

done < "$UNWANTED_FILE"

#
# Clean up any __pyacache_ files
#

find "${APP_DIR}" -name __pycache__ -exec rm -rf {} +

msg "Cleanup complete!"

userID="userID.sh"

if [ ! -f "$userID" ]; then
    echo "No password & credential file to continue with codesig and App store verification .. done"
    exit 1
else
    source userID.sh    
fi

#
# now codesign
#


msg "codesigning ${APP_DIR} --deep"    
codesign --force --deep --verbose --timestamp --options=runtime --sign "$appleCredential" "$APP_DIR" || die "FAIL: codesign failed."
    
#
# Sign QtWebEngineProcess with its bundled entitlements as --deep on APP overwrites this, making plots appear empty in notarized builds
#

WEBENGINE_PROCESS="${APP_DIR}/Contents/Frameworks/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app"
msg "codesigning ${WEBENGINE_PROCESS}"
WEBENGINE_ENTITLEMENTS="${WEBENGINE_PROCESS}/Contents/Resources/QtWebEngineProcess.entitlements"
if [ -f "${WEBENGINE_ENTITLEMENTS}" ]; then
    codesign --force --timestamp --options=runtime \
             --entitlements "${WEBENGINE_ENTITLEMENTS}" \
             --sign "$appleCredential" "${WEBENGINE_PROCESS}" || die "FAIL: QtWebEngineProcess codesign failed."
fi

#
# build dmg bundle
#

DMG_METHOD="NEW"
if [ "${DMG_METHOD}" = "NEW" ]; then

    (
        cd "${BUILD_DIR}"
        mkdir app
        mv "${APP_NAME}" app
        set -x

        ../macInstall/create-dmg \
            --volname "${PROJECT_NAME}" \
            --background "../macInstall/background3.png" \
            --window-pos 200 120 \
            --window-size 600 350 \
            --no-internet-enable \
            --icon-size 125 \
            --icon "${APP_NAME}" 125 130 \
            --hide-extension "${APP_NAME}" \
            --app-drop-link 450 130 \
            --codesign "$appleCredential" \
            "EE-UQ_Mac_Download_${ARCH}.dmg" \
            "app"

        set +x
        mv "./app/${APP_NAME}" ./
        rm -fr app
    )

else

    (
        cd "${BUILD_DIR}"
        hdiutil create "EE-UQ_Mac_Download_${ARCH}.dmg" -fs HFS+ -srcfolder "./${APP_NAME}" -format UDZO -volname "${APP_NAME}"
    )

fi


#
# now xcrun stuff to get it signed and stapled
#

msg "DMG built .. now uploading to Apple for validation"

xcrun notarytool submit "${BUILD_DIR}/EE-UQ_Mac_Download_${ARCH}.dmg" --apple-id "$appleID" --password "$appleAppPassword" --team-id "$appleCredential"

msg "Manual Mode! ... "
echo ""
echo "last command returns id: ID,  wait awhile for apple to verify then check status & log with: "
echo "   .. info for status and if succesull staple, otherwise look at log file and correct!"
echo "xcrun notarytool info ID  --apple-id $appleID --password $appleAppPassword --team-id $appleCredential"
echo "xcrun notarytool log ID --apple-id $appleID --password $appleAppPassword --team-id $appleCredential"
echo ""
echo "Finally staple the dmg"
echo "xcrun stapler staple  ${BUILD_DIR}/EE-UQ_Mac_Download_${ARCH}.dmg"


echo "Release Build complete!"

