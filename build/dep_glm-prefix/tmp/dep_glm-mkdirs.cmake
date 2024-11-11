# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/src/dep_glm"
  "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/src/dep_glm-build"
  "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix"
  "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/tmp"
  "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/src/dep_glm-stamp"
  "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/src"
  "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/src/dep_glm-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/src/dep_glm-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sihwan/Programming/shaderPixel/build/dep_glm-prefix/src/dep_glm-stamp${cfgdir}") # cfgdir has leading slash
endif()
