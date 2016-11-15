# general script to create helper library from folder
##############################################################################
#input variables
# LIB - library name
# LIB_FOLDER - library folder
# LIB_FLAGS - compilation flags
# ARGN - true expression to install target lib and header
##############################################################################
#output variables
#LIB_INCLUDE_DIRS
#LIB_LIBRARIES
##############################################################################
cmake_minimum_required(VERSION 2.8)

MACRO(add_sublibrary LIB LIB_FOLDER LIB_FLAGS)
# collect source files
file(GLOB ${LIB}_SOURCES ${LIB_FOLDER}/*.cpp ${LIB_FOLDER}/*.c)
file(GLOB ${LIB}_HEADERS ${LIB_FOLDER}/*.hpp ${LIB_FOLDER}/*.h)

# output an library
add_library(${LIB} STATIC ${${LIB}_SOURCES} ${${LIB}_HEADERS})
# set header files for install
set_target_properties(
    ${LIB} PROPERTIES
    PUBLIC_HEADER "${${LIB}_HEADERS}"
    COMPILE_FLAGS "${LIB_FLAGS}"
)
# add includes so they get added automatically to targets depending on this
target_include_directories(${LIB} SYSTEM PUBLIC ${LIB_FOLDER})

#set output variables for linking
string(TOUPPER ${LIB} LIB_CAPS)
set(${LIB_CAPS}_LIBRARIES ${LIB} CACHE STRING " library filenames")
# set(${LIB_CAPS}_INCLUDE_DIRS ${LIB_FOLDER} CACHE STRING "include directories")

# Cannot use ARGN directly with list() command.
set(extra_macro_args ${ARGN})
# check if first optional argument is true
list(GET extra_macro_args 0 INST)
if(${INST})
  install(
      TARGETS ${LIB}
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib
      PUBLIC_HEADER DESTINATION include/${LIB}
  )
endif(${INST})

ENDMACRO(add_sublibrary)