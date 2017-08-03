cmake_minimum_required(VERSION 3.0.2)
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
set(GLOBAL_EXT_DIR ${CMAKE_SOURCE_DIR}/external)
include(FindDependency)
include(AddSubLibrary)
################################
# zmq
set(ZMQ_INCLUDE_SEARCH_DIR "/opt/zmq/current/include")
set(ZMQ_LIBRARY_SEARCH_DIR "/opt/zmq/current/lib")
find_dependency_custom(ZMQ "zmq.h" "zmq")
################################
# boost
set(BOOST_INCLUDEDIR "/opt/boost/boost_1_55_0/include")
set(BOOST_LIBRARYDIR "/opt/boost/boost_1_55_0/lib")
find_package(Boost 1.54.0 REQUIRED COMPONENTS thread system)
set(BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} CACHE PATH "boost include dir")
set(BOOST_LIBRARIES ${Boost_LIBRARIES} CACHE PATH "boost libraries")
include_directories(SYSTEM ${BOOST_INCLUDE_DIRS})
################################
#CGAL
set(CGAL_INCLUDE_SEARCH_DIR "/opt/cgal/include")
set(CGAL_LIBRARY_SEARCH_DIR "/opt/cgal/lib/")
find_dependency_custom(CGAL "CGAL/basic.h" "CGAL")
################################
#gmp
set(GMP_INCLUDE_SEARCH_DIR "/opt/gmp/include")
set(GMP_LIBRARY_SEARCH_DIR "/opt/gmp/lib")
find_dependency_custom(GMP "gmp.h" "gmp")
################################
#mpfr
set(MPFR_INCLUDE_SEARCH_DIR "/opt/mpfr/include")
set(MPFR_LIBRARY_SEARCH_DIR "/opt/mpfr/lib")
find_dependency_custom(MPFR "mpfr.h" "mpfr")
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
set(GLM_INCLUDE_DIR /external/${GLM_DIRECTORY})
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/${GLM_DIRECTORY})
################################
# squish
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/squish)
################################
set(OPTION_BUILD_TESTS OFF CACHE STRING "" FORCE)
# glbinding
set(GLB_DIRECTORY "glbinding-2.0.0")
set(glbinding_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/${GLB_DIRECTORY}")
add_subdirectory(${glbinding_DIR})
include_directories(SYSTEM ${glbinding_DIR}/source/glbinding/include)
include_directories(SYSTEM ${GLBINDING_EXPORT_DIRS})
include_directories(SYSTEM external)
################################
# globjects
set(GLOW_DIRECTORY "globjects-0.5.0")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/${GLOW_DIRECTORY})
################################
# FastDXT
add_sublibrary(fastdxt ${CMAKE_CURRENT_SOURCE_DIR}/external/fastdxt "-O3 -funroll-loops -fomit-frame-pointer -fPIC -msse2 -DDXT_INTR -w")
################################
# gloost
add_sublibrary(gloost ${CMAKE_CURRENT_SOURCE_DIR}/external/gloost "-w")
################################
# imgui
add_sublibrary(imgui ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-1.49 "-w")
target_link_libraries(imgui glfw)
################################
# svg
add_sublibrary(svg ${CMAKE_CURRENT_SOURCE_DIR}/external/svg "-w")
################################
# catch
set(CATCH_DIRECTORY catch-1.1)
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/${CATCH_DIRECTORY})

################################
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

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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

################################
#if user didnt set install dir, override it and write it to the cache -> Type and description necessary,
#overwrite variable, not just write it to cache
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# copy over shaders
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glsl 
        DESTINATION bin/
        FILES_MATCHING 
        PATTERN "*.fs"
        PATTERN "*.vs"
        PATTERN "*.gs"
        PATTERN "*.glsl"
)
