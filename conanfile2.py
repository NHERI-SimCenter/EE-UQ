from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy
import os

class EEUQConan(ConanFile):
    name = "EE_UQ"
    license = "BSD-3-Clause"
    author = "NHERI SimCenter"
    url = "https://github.com/NHERI-SimCenter/EE-UQ"
    description = "NHERI SimCenter EE-UQ Tool"

    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("zlib/1.3.1")
        if self.settings.os != "Linux":
            self.requires("libcurl/8.12.1")
            
    def layout(self):
        cmake_layout(self)

    def generate(self):
        # Generate toolchain + find_package config files        
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

        # The following solves the "DLL not found" error during local development
        if self.settings.os == "Windows":
            # For CMake, the binary usually ends up in build/Release or build/Debug
            bindir = os.path.join(self.build_folder, str(self.settings.build_type))
            for dep in self.dependencies.values():
                copy(self, "*.dll", dep.cpp_info.bindirs[0], bindir)
