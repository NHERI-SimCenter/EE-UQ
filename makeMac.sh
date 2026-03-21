#!/bin/bash

# Define a helper to print error and stop without closing the terminal
die() {
    echo "FAIL $1"
    return 1 2>/dev/null || exit 1
}

# Check for Conan (Added a space after ! for safety)
if ! command -v conan &> /dev/null; then
    die "'conan' command not found. Please install it first."
fi

# Check for CMakeLists.txt before doing anything
[ -f "CMakeLists.txt" ] || die "CMakeLists.txt not found in $(pwd)"

echo "Cleaning up old build directory..."
rm -rf build

echo "Running conan install..."
# copy conanfile2 to conanfile till i remove old qmake build
if [ -f "conanfile2.py" ]; then
    mv conanfile.py conanfile.old
    cp conanfile2.py conanfile.py
fi
conan install . --build missing -s build_type=Release || die "Conan install failed."
if [ -f "conanfile2.py" ]; then
    mv conanfile.old conanfile.py
fi

# Use a subshell for the build steps
(
    mkdir -p build/Release
    cd build/Release || exit 1
    
    echo "Configuring CMake..."
    cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release || exit 1
    
    echo "Building with 8 cores..."
    cmake --build . --parallel 8 || exit 1
)

# Catch subshell failure
if [ $? -ne 0 ]; then
    die "Build failed."
fi

# Copy Examples folder
cp -fr Examples ./build/Release/EE_UQ.app/Contents/MacOS/
rm -fr ./build/Release/EE_UQ.app/Contents/MacOS/Examples/.archive
rm -fr ./build/Release/EE_UQ.app/Contents/MacOS/Examples/.aurore
rm -fr ./build/Release/EE_UQ.app/Contents/MacOS/Examples/.gitignore

echo "Build complete!"
