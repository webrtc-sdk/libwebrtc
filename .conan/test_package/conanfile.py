#!/usr/bin/env python

from conan import ConanFile
from conan.tools.cmake import CMake

class TestPackageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"


    def requirements(self):
        self.requires("libwebrtc/0.0.1")


    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()


    def test(self):
        pass
