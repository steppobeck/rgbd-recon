#general script to search for dependency libraries and  headers 
##############################################################################
#input variables
#DEP
#DEP_HEADER
#DEP_LIB
#custom paths
#DEP_INCLUDE_SEARCH_DIR}
#DEP_LIBRARY_SEARCH_DIR}
##############################################################################
#output variables
#DEP_INCLUDE_DIRS
#DEP_LIBRARY
MACRO(find_dependency DEP DEP_HEADER DEP_LIB)
##############################################################################
# search paths
##############################################################################
SET(${DEP}_INCLUDE_SEARCH_DIRS
  ${GLOBAL_EXT_DIR}/inc/${DEP}
  ${${DEP}_INCLUDE_DIRS}
  ${${DEP}_INCLUDE_SEARCH_DIR}
  "/usr/include"
  "/usr/share/include"
  "/usr/include/x86_64-linux-gnu/"
)

SET(${DEP}_LIBRARY_SEARCH_DIRS
  ${GLOBAL_EXT_DIR}/lib
  ${${DEP}_LIBRARY_DIRS}
  ${${DEP}_LIBRARY_SEARCH_DIR}
  "/usr/lib/"
  "/usr/lib/x86_64-linux-gnu/"
)
##############################################################################
# feedback to provide user-defined paths to search for ${DEP}
##############################################################################
MACRO (request_dep_search_directories)
    
  IF ( NOT ${DEP}_INCLUDE_DIRS AND NOT ${DEP}_LIBRARY_DIRS )
    SET(${DEP}_INCLUDE_SEARCH_DIR "Please provide ${DEP} include path." CACHE PATH "path to ${DEP} headers.")
    SET(${DEP}_LIBRARY_SEARCH_DIR "Please provide ${DEP} library path." CACHE PATH "path to ${DEP} libraries.")
    MESSAGE(FATAL_ERROR "find_${DEP}.cmake: unable to find ${DEP}.")
  ENDIF ( NOT ${DEP}_INCLUDE_DIRS AND NOT ${DEP}_LIBRARY_DIRS )

  IF ( NOT ${DEP}_INCLUDE_DIRS )
    SET(${DEP}_INCLUDE_SEARCH_DIR "Please provide ${DEP} include path." CACHE PATH "path to ${DEP} headers.")
    MESSAGE(FATAL_ERROR "find_${DEP}.cmake: unable to find ${DEP} headers.")
  ELSE ( NOT ${DEP}_INCLUDE_DIRS )
    UNSET(${DEP}_INCLUDE_SEARCH_DIR CACHE)
  ENDIF ( NOT ${DEP}_INCLUDE_DIRS )

  IF ( NOT ${DEP}_LIBRARY_DIRS )
    SET(${DEP}_LIBRARY_SEARCH_DIR "Please provide ${DEP} library path." CACHE PATH "path to ${DEP} libraries.")
    MESSAGE(FATAL_ERROR "find_${DEP}.cmake: unable to find ${DEP} libraries.")
  ELSE ( NOT ${DEP}_LIBRARY_DIRS )
    UNSET(${DEP}_LIBRARY_SEARCH_DIR CACHE)
  ENDIF ( NOT ${DEP}_LIBRARY_DIRS ) 

ENDMACRO (request_dep_search_directories)

##############################################################################
# search
##############################################################################
message(STATUS "Looking for ${DEP}")

IF (NOT ${DEP}_INCLUDE_DIRS)

  SET(_${DEP}_FOUND_INC_DIRS "")
  FOREACH(_SEARCH_DIR ${${DEP}_INCLUDE_SEARCH_DIRS})
    FIND_PATH(_CUR_SEARCH
      NAMES ${DEP_HEADER}
        PATHS ${_SEARCH_DIR}
        NO_DEFAULT_PATH)
    IF (_CUR_SEARCH)
      LIST(APPEND _${DEP}_FOUND_INC_DIRS ${_CUR_SEARCH})
    ENDIF(_CUR_SEARCH)
    SET(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")
  ENDFOREACH(_SEARCH_DIR ${${DEP}_INCLUDE_SEARCH_DIRS})

  IF (NOT _${DEP}_FOUND_INC_DIRS)
    request_dep_search_directories()
  ENDIF (NOT _${DEP}_FOUND_INC_DIRS)

  FOREACH(_INC_DIR ${_${DEP}_FOUND_INC_DIRS})
    SET(${DEP}_INCLUDE_DIRS ${${DEP}_INCLUDE_DIRS} ${_INC_DIR} CACHE PATH "${DEP} include directory.")
  ENDFOREACH(_INC_DIR ${_${DEP}_FOUND_INC_DIRS})

ENDIF (NOT ${DEP}_INCLUDE_DIRS)

IF(UNIX)
  SET(${DEP}_LIB_FILENAME "${DEP_LIB}.so")
ELSEIF(WIN32)
  SET(${DEP}_LIB_FILENAME "${DEP_LIB}.lib")
ENDIF(UNIX)

IF ( NOT ${DEP}_LIBRARY_DIRS )

  SET(_${DEP}_FOUND_LIB_DIR "")
  SET(_${DEP}_POSTFIX "")

  FOREACH(_SEARCH_DIR ${${DEP}_LIBRARY_SEARCH_DIRS})
    FIND_PATH(_CUR_SEARCH
      NAMES ${${DEP}_LIB_FILENAME}
        PATHS ${_SEARCH_DIR}
        PATH_SUFFIXES release debug
        NO_DEFAULT_PATH)
    IF (_CUR_SEARCH)
      LIST(APPEND _${DEP}_FOUND_LIB_DIR ${_SEARCH_DIR})
    ENDIF(_CUR_SEARCH)
    SET(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")
  ENDFOREACH(_SEARCH_DIR ${${DEP}_LIBRARY_SEARCH_DIRS})

  IF (NOT _${DEP}_FOUND_LIB_DIR)
    request_dep_search_directories()
  ELSE (NOT _${DEP}_FOUND_LIB_DIR)
    SET(${DEP}_LIBRARY_DIRS ${_${DEP}_FOUND_LIB_DIR} CACHE PATH "The ${DEP} library directory")
  ENDIF (NOT _${DEP}_FOUND_LIB_DIR)

  FOREACH(_LIB_DIR ${_${DEP}_FOUND_LIB_DIR})
    LIST(APPEND _${DEP}_LIBRARIES "${_LIB_DIR}/${${DEP}_LIB_FILENAME}")
  ENDFOREACH(_LIB_DIR ${_${DEP}_FOUND_INC_DIRS})

  IF (_${DEP}_FOUND_LIB_DIR)
    SET(${DEP}_LIBRARIES ${_${DEP}_LIBRARIES} CACHE FILEPATH "The ${DEP} library filename.")
  ENDIF (_${DEP}_FOUND_LIB_DIR)

ENDIF ( NOT ${DEP}_LIBRARY_DIRS )

##############################################################################
# verify
##############################################################################
IF ( NOT ${DEP}_INCLUDE_DIRS OR NOT ${DEP}_LIBRARY_DIRS )
  request_dep_search_directories()
  message(STATUS "Looking for ${DEP} - not found")
ELSE ( NOT ${DEP}_INCLUDE_DIRS OR NOT ${DEP}_LIBRARY_DIRS ) 
  UNSET(${DEP}_INCLUDE_SEARCH_DIR CACHE)
  UNSET(${DEP}_LIBRARY_SEARCH_DIR CACHE)
  # MESSAGE(STATUS "--  found matching ${DEP} version")
  message(STATUS "Looking for ${DEP} - found")
ENDIF ( NOT ${DEP}_INCLUDE_DIRS OR NOT ${DEP}_LIBRARY_DIRS )

ENDMACRO(find_dependency)
