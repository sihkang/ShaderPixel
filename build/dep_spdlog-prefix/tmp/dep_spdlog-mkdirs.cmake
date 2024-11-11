# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/src/dep_spdlog"
  "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/src/dep_spdlog-build"
  "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix"
  "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/tmp"
  "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/src/dep_spdlog-stamp"
  "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/src"
  "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/src/dep_spdlog-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/src/dep_spdlog-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sihwan/Programming/shaderPixel/build/dep_spdlog-prefix/src/dep_spdlog-stamp${cfgdir}") # cfgdir has leading slash
endif()
