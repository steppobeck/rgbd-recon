cmake_minimum_required(VERSION 2.8)
project(rgbd-calib)


if (UNIX)
  find_package(PkgConfig)
endif(UNIX)

# Location where cmake first looks for modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)



if(MSVC)
  set(BINARY_DIRECTORY build)
endif()

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel") AND UNIX))
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

include(FindDependency)
################################
# zmq
include(FindZMQ)
# OpenCV
include(FindOpenCV)
# boost
include(FindBoost)
#CGAL
set(CGAL_INCLUDE_SEARCH_DIR "/opt/cgal/include")
set(CGAL_LIBRARY_SEARCH_DIR "/opt/cgal/lib/")
find_dependency(CGAL "CGAL/basic.h" "libCGAL")
include_directories(${CGAL_INCLUDE_DIRS})
#gmp
set(GMP_INCLUDE_SEARCH_DIR "/opt/gmp/include")
set(GMP_LIBRARY_SEARCH_DIR "/opt/gmp/lib")
find_dependency(GMP "gmp.h" "libgmp")
include_directories(${GMP_INCLUDE_DIRS})
#mpfr
set(MPFR_INCLUDE_SEARCH_DIR "/opt/mpfr/include")
set(MPFR_LIBRARY_SEARCH_DIR "/opt/mpfr/lib")
find_dependency(MPFR "mpfr.h" "libmpfr")
include_directories(${MPFR_INCLUDE_DIRS})

add_definitions(-DFENSTERCHEN_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
option(FENSTERCHEN_IN_SOURCE_BUILD "FENSTERCHEN_IN_SOURCE_BUILD" OFF)
if(FENSTERCHEN_IN_SOURCE_BUILD)
  add_definitions(-DFENSTERCHEN_BINARY_DIR="../")
else()
  add_definitions(-DFENSTERCHEN_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)
add_definitions(-DGLEW_STATIC)

set(GLFW_DIRECTORY glfw-3.0.3)
set(GLFW_INSTALL OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE STRING "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLFW_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLFW_DIRECTORY}/include)

set(GLM_DIRECTORY glm-0.9.5.3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLM_DIRECTORY})

################################
# zmq
include_directories(SYSTEM ${ZMQ_INCLUDE_DIRS})
################################
# OpenCV
include_directories(SYSTEM ${OPENCV_INCLUDE_DIRS})
################################
# boost
include_directories(SYSTEM ${BOOST_INCLUDE_DIRS})
################################
# cgal
include_directories(SYSTEM ${CGAL_INCLUDE_DIRS})
################################
# squish
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/squish)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/squish)
################################
# FastDXT
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/fastdxt)
include_directories(${FASTDXT_INCLUDE_DIRS})
################################
# gloost
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/gloost)
include_directories(${GLOOST_INCLUDE_DIRS})
################################
# svg
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/svg)
include_directories(${SVG_INCLUDE_DIRS})

################################
# Add libraries to executables

################################
# zmq and other libraries
set(LIBRARIES glfw GLU glut ${GLFW_LIBRARIES} ${ZMQ_LIBRARIES} ${OPENCV_LIBRARIES} ${CGAL_LIBRARIES} ${BOOST_LIBRARIES} ${SQUISH_LIBRARIES} ${FASTDXT_LIBRARIES} ${GLOOST_LIBRARIES} ${SVG_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES})
set(CATCH_DIRECTORY catch-1.1)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${CATCH_DIRECTORY})
################################
# zmq
link_directories(${ZMQ_LIBRARY_DIRS})
# OpenCV
link_directories(${OPENCV_LIBRARY_DIRS})
# boost
link_directories(${BOOST_LIBRARY_DIRS})
################################
# Add output directory

if(MSVC)
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(COPY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Debug/ )
  else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(COPY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Release/ )
  endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
else(MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Debug )
  else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/Release )
  endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(MSVC)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/framework)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/framework/calibration)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/framework/navigation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/framework/measurement)
add_subdirectory(framework)

add_subdirectory(source)
