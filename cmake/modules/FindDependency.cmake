#general script to search for dependency libraries and headers 
##############################################################################
#input variables
# DEP - dependency id
# DEP_HEADER - header file to search for
# DEP_LIBRARY - library file to search for
# ARGN - additional library names to put int DEP_LIBRARIES
##############################################################################
#path hints
#DEP_INCLUDE_SEARCH_DIR
#DEP_LIBRARY_SEARCH_DIR
##############################################################################
#output variables
#DEP_INCLUDE_DIRS
#DEP_LIBRARIES
#DEP_LIBRARY_DIRS
##############################################################################
MACRO(find_dependency DEP DEP_HEADER DEP_LIB)
##############################################################################
# search paths
##############################################################################
# search before global environment paths
list(APPEND ${DEP}_INCLUDE_SEARCH_HINTS
  ${${DEP}_INCLUDE_DIRS}
  ${${DEP}_INCLUDE_SEARCH_DIR}
  ${GLOBAL_EXT_DIR}/inc/${DEP}
  ${GLOBAL_EXT_DIR}/${DEP}/include
)
# search after global environment paths
list(APPEND ${DEP}_INCLUDE_SEARCH_PATHS
  "/usr/include"
  "/usr/share/include"
  "/usr/include/x86_64-linux-gnu/"
)

list(APPEND ${DEP}_LIBRARY_SEARCH_HINTS
  ${${DEP}_LIBRARY_DIRS}
  ${${DEP}_LIBRARY_SEARCH_DIR}
  ${GLOBAL_EXT_DIR}/lib
  ${GLOBAL_EXT_DIR}/${DEP}/lib
)
list(APPEND ${DEP}_LIBRARY_SEARCH_PATHS
  "/usr/lib/"
  "/usr/lib/x86_64-linux-gnu/"
)
##############################################################################
# macros
##############################################################################
# feedback to provide user-defined paths to search for ${DEP}
MACRO(request_include_search_dirs)
  # if hints were provided, display
  if (DEFINED ${DEP}_INCLUDE_SEARCH_DIR)
    set(${DEP}_INCLUDE_SEARCH_DIR ${${DEP}_INCLUDE_SEARCH_DIR} CACHE PATH "path to ${DEP} headers.")
    # if not, request them
  else()
    set(${DEP}_INCLUDE_SEARCH_DIR "Please provide ${DEP} include path." CACHE PATH "path to ${DEP} headers.")
  endif()
ENDMACRO()

MACRO(request_lib_search_dirs)
  # if hints were provided, display
  if (DEFINED ${DEP}_LIBRARY_SEARCH_DIR)
    set(${DEP}_LIBRARY_SEARCH_DIR ${${DEP}_LIBRARY_SEARCH_DIR} CACHE PATH "path to ${DEP} libraries.")
    # if not, request them
  else()
    set(${DEP}_LIBRARY_SEARCH_DIR "Please provide ${DEP} library path." CACHE PATH "path to ${DEP} libraries.")
  endif()
ENDMACRO()
##############################################################################
# search
##############################################################################
message(STATUS "Looking for ${DEP}")
# search for include directory
if (NOT ${DEP}_INCLUDE_DIRS)

  find_path(_CUR_SEARCH
    NAMES ${DEP_HEADER}
    HINTS ${${DEP}_INCLUDE_SEARCH_HINTS}
    PATHS ${${DEP}_INCLUDE_SEARCH_PATHS}
    PATH_SUFFIXES release debug
  )

  if(_CUR_SEARCH)
    set(${DEP}_INCLUDE_DIRS ${_CUR_SEARCH} CACHE PATH "${DEP} include directory.")
  else()
    request_include_search_dirs()
    message(FATAL_ERROR "Unable to find ${DEP} headers, please specify the correct ${DEP}_INCLUDE_SEARCH_DIR.")
  endif()
  set(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")

endif (NOT ${DEP}_INCLUDE_DIRS)

# search for libraries
if (NOT ${DEP}_LIBS_FOUND)
  # create list of libs from lib and optional arguments
  set(${DEP}_LIB_LIST ${DEP_LIB})
  list(APPEND ${DEP}_LIB_LIST ${ARGN})
  # accumulate library list
  foreach(_SEARCH_LIB ${${DEP}_LIB_LIST})
    # search only for lib if it hasnt already been found
    list(FIND ${DEP}_FOUND_LIBS ${_SEARCH_LIB} _INDEX)
    if(${_INDEX} LESS 0)
      find_library(_CUR_SEARCH
        NAMES ${_SEARCH_LIB}
        HINTS ${${DEP}_LIBRARY_SEARCH_HINTS}
        PATHS ${${DEP}_LIBRARY_SEARCH_PATHS}
        PATH_SUFFIXES release debug
      )
      # react to search result
      if(_CUR_SEARCH)
          # add library and path to output lists 
          set(${DEP}_LIBRARIES ${${DEP}_LIBRARIES} ${_CUR_SEARCH} CACHE FILEPATH "The ${DEP} library filenames." FORCE)
          get_filename_component(_${DEP}_LIB_PATH ${_CUR_SEARCH} DIRECTORY)
          set(${DEP}_LIBRARY_DIRS ${${DEP}_LIBRARY_DIRS} ${_${DEP}_LIB_PATH} CACHE PATH "The ${DEP} library directories." FORCE)
          # mark library as found
          set(${DEP}_FOUND_LIBS ${${DEP}_FOUND_LIBS} ${_SEARCH_LIB} CACHE INTERNAL "internal use")
        # reset search status for next search
        set(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")
      else()
        set(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")
        request_lib_search_dirs()
        # stops processing and exits loop
        message(FATAL_ERROR "Unable to find ${DEP} library '${_SEARCH_LIB}', please specify the correct ${DEP}_LIBRARY_SEARCH_DIR.")
      endif()
    endif()
  endforeach()
  # if statement is reached, all libraries were found
  set(${DEP}_LIBS_FOUND TRUE CACHE INTERNAL "internal use")
endif()

##############################################################################
# verify
##############################################################################
if(${DEP}_INCLUDE_DIRS AND ${DEP}_LIBS_FOUND)
  unset(${DEP}_INCLUDE_SEARCH_DIR CACHE)
  unset(${DEP}_LIBRARY_SEARCH_DIR CACHE)
  message(STATUS "Looking for ${DEP} - found")
  # create imported library if version is high enough
  if(${CMAKE_MAJOR_VERSION} GREATER 2 AND ${CMAKE_MINOR_VERSION} GREATER -1 AND ${CMAKE_PATCH_VERSION} GREATER 1)
    add_library(${DEP_LIB} INTERFACE IMPORTED GLOBAL)
    set_property(TARGET ${DEP_LIB} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${${DEP}_INCLUDE_DIRS})
    set_property(TARGET ${DEP_LIB} PROPERTY INTERFACE_LINK_LIBRARIES "${${DEP}_LIBRARIES}")
  endif()
else() 
  message(STATUS "Looking for ${DEP} - not found")
endif()

ENDMACRO(find_dependency)