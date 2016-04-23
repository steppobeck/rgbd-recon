cmake_minimum_required(VERSION 2.8)
project(rgbd-calib)

#set the build type if its not set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if (UNIX)
  find_package(PkgConfig)
endif(UNIX)

# Location where cmake first looks for modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

include(FindDependency)
################################
# zmq
set(ZMQ_INCLUDE_SEARCH_DIR "/opt/zmq/current/include")
set(ZMQ_LIBRARY_SEARCH_DIR "/opt/zmq/current/lib")
find_dependency(ZMQ "zmq.h" "libzmq")
include_directories(SYSTEM ${ZMQ_INCLUDE_DIRS})
################################
# OpenCV
# include(FindOpenCV)
# link_directories(${OPENCV_LIBRARY_DIRS})
# list(APPEND INCLUDE_DIRS ${OPENCV_INCLUDE_DIRS})
################################
# boost
include(FindBoost)
link_directories(${BOOST_LIBRARY_DIRS})
include_directories(SYSTEM ${BOOST_INCLUDE_DIRS})
################################
#CGAL
set(CGAL_INCLUDE_SEARCH_DIR "/opt/cgal/include")
set(CGAL_LIBRARY_SEARCH_DIR "/opt/cgal/lib/")
find_dependency(CGAL "CGAL/basic.h" "libCGAL")
include_directories(SYSTEM ${CGAL_INCLUDE_DIRS})
################################
#gmp
set(GMP_INCLUDE_SEARCH_DIR "/opt/gmp/include")
set(GMP_LIBRARY_SEARCH_DIR "/opt/gmp/lib")
find_dependency(GMP "gmp.h" "libgmp")
include_directories(SYSTEM ${GMP_INCLUDE_DIRS})
################################
#mpfr
set(MPFR_INCLUDE_SEARCH_DIR "/opt/mpfr/include")
set(MPFR_LIBRARY_SEARCH_DIR "/opt/mpfr/lib")
find_dependency(MPFR "mpfr.h" "libmpfr")
include_directories(SYSTEM ${MPFR_INCLUDE_DIRS})
################################
# glfw
set(GLFW_DIRECTORY glfw-3.0.3)
set(GLFW_INSTALL OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE STRING "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLFW_DIRECTORY})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLFW_DIRECTORY}/include)
################################
# glm
set(GLM_DIRECTORY glm-0.9.5.3)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/${GLM_DIRECTORY})
################################
# squish
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/squish)
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external)
################################
# FastDXT
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/fastdxt)
# suppress warnings
target_compile_options(libfastdxt PRIVATE -w)
include_directories(SYSTEM ${FASTDXT_INCLUDE_DIRS})
################################
# gloost
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/gloost)
include_directories(SYSTEM ${GLOOST_INCLUDE_DIRS})
################################
# svg
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/svg)
include_directories(SYSTEM ${SVG_INCLUDE_DIRS})
################################
# catch
set(CATCH_DIRECTORY catch-1.1)
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/${CATCH_DIRECTORY})

################################
# set link libraries
set(LIBRARIES glfw GLU glut ${GLFW_LIBRARIES} ${ZMQ_LIBRARIES} ${CGAL_LIBRARIES} ${BOOST_LIBRARIES} ${SQUISH_LIBRARIES} ${FASTDXT_LIBRARIES} ${GLOOST_LIBRARIES} ${SVG_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES})

add_definitions(-DGLEW_STATIC)
# to prevent CGAL assertion fail 
add_definitions(-frounding-math)

################################
# activate C++ 11
if(NOT MSVC)
    add_definitions(-std=c++11)
    # show warnings
    add_definitions(-Wall -Wextra)
    # force linking  with c++11 lib
    if(APPLE)
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        add_definitions(-stdlib=libc++)
    endif()
else()
    # build in parallel, show warnings
    add_definitions(/MP /W3)
endif()

# Add output directory
if(MSVC)
  set(BINARY_DIRECTORY build)
endif()
# MSVC & Xcode automatically create the build-type folders
if(MSVC OR CMAKE_GENERATOR STREQUAL Xcode)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
  SET(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR})
else()
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  SET(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif()

# framework
add_subdirectory(framework)
# applications
add_subdirectory(source)

################################
# suppress displaying of external lib options  
#GLFW
mark_as_advanced(GLFW_BUILD_DOCS GLFW_BUILD_TESTS GLFW_INSTALL GLFW_BUILD_EXAMPLES
 GLFW_DOCUMENT_INTERNALS GLFW_USE_EGL LIB_SUFFIX BUILD_SHARED_LIBS)
#squish
mark_as_advanced(BUILD_SQUISH_WITH_SSE2 BUILD_SQUISH_WITH_ALTIVEC
 BUILD_SHARED_LIBS BUILD_SQUISH_EXTRA)
