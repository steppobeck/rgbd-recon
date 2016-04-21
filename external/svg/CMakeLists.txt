# general script to create small library
cmake_minimum_required(VERSION 2.8)
# get current folder name
get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" FOLDER_NAME ${FOLDER_NAME})
# allows access to environment variables with the name 
project(${FOLDER_NAME})

# collect source files
file(GLOB ${PROJECT_NAME}_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# outputs an executable
add_library(lib${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})

#set output variables for linking
string(TOUPPER ${FOLDER_NAME} PROJECT_CAPS)
set(${PROJECT_CAPS}_LIBRARIES lib${PROJECT_NAME} CACHE STRING " librarie filnames")
set(${PROJECT_CAPS}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "include directories")