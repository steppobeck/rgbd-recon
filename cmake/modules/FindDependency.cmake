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
list(APPEND ${DEP}_INCLUDE_SEARCH_DIRS
  ${${DEP}_INCLUDE_DIRS}
  ${${DEP}_INCLUDE_SEARCH_DIR}
  ${GLOBAL_EXT_DIR}/inc/${DEP}
  "/usr/include"
  "/usr/share/include"
  "/usr/include/x86_64-linux-gnu/"
)

# search before global paths
list(APPEND ${DEP}_LIBRARY_SEARCH_HINTS
  ${${DEP}_LIBRARY_DIRS}
  ${${DEP}_LIBRARY_SEARCH_DIR}
  ${GLOBAL_EXT_DIR}/lib
  ${GLOBAL_EXT_DIR}/${DEP}/lib
)
# search after global paths
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

  set(_${DEP}_FOUND_INC_DIRS "")
  foreach(_SEARCH_DIR ${${DEP}_INCLUDE_SEARCH_DIRS})
    find_path(_CUR_SEARCH
      NAMES ${DEP_HEADER}
        PATHS ${_SEARCH_DIR}
        NO_DEFAULT_PATH)
    # stop search once dir is found
    if (_CUR_SEARCH)
      list(APPEND _${DEP}_FOUND_INC_DIRS ${_CUR_SEARCH})
      break()
    endif(_CUR_SEARCH)
  endforeach(_SEARCH_DIR ${${DEP}_INCLUDE_SEARCH_DIRS})
  # reset search status for next search
  set(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")

  if (NOT _${DEP}_FOUND_INC_DIRS)
    request_include_search_dirs()
    message(FATAL_ERROR "Unable to find ${DEP} headers.")
  endif (NOT _${DEP}_FOUND_INC_DIRS)

  foreach(_INC_DIR ${_${DEP}_FOUND_INC_DIRS})
    set(${DEP}_INCLUDE_DIRS ${${DEP}_INCLUDE_DIRS} ${_INC_DIR} CACHE PATH "${DEP} include directory.")
  endforeach(_INC_DIR ${_${DEP}_FOUND_INC_DIRS})

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
        message(FATAL_ERROR "Unable to find ${DEP} library '${_SEARCH_LIB}'")
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
else() 
  message(STATUS "Looking for ${DEP} - not found")
endif()

ENDMACRO(find_dependency)