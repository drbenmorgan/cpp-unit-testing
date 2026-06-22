#-----------------------------------------------------------------------
# - Top Level CMakeLists.txt for CCPTEPPTest Build
#-----------------------------------------------------------------------
# - Enforce an out-of-source builds before anything else
#
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
  message(STATUS "CCPTEPPTest requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_CURRENT_BINARY_DIR} first:")
  message(STATUS "CMakeCache.txt")
  message(STATUS "CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# - Define CMake requirements
cmake_minimum_required(VERSION 3.26...4.2)
project(CCPTEPPTest)


