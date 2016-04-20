##############################################################################
# search paths
##############################################################################
SET(ZMQ_INCLUDE_SEARCH_DIRS
  ${GLOBAL_EXT_DIR}/inc/zmq
  ${ZMQ_INCLUDE_DIRS}
  ${ZMQ_INCLUDE_SEARCH_DIR}
  "/opt/zmq/current/include"
)

SET(ZMQ_LIBRARY_SEARCH_DIRS
  ${GLOBAL_EXT_DIR}/lib
  ${ZMQ_LIBRARY_DIRS}
  ${ZMQ_LIBRARY_SEARCH_DIR}
  "/opt/zmq/current/lib"
)

##############################################################################
# feedback to provide user-defined paths to search for zmq
##############################################################################
MACRO (request_zmq_search_directories)
    
  IF ( NOT ZMQ_INCLUDE_DIRS AND NOT ZMQ_LIBRARY_DIRS )
    SET(ZMQ_INCLUDE_SEARCH_DIR "Please provide zmq include path." CACHE PATH "path to zmq headers.")
    SET(ZMQ_LIBRARY_SEARCH_DIR "Please provide zmq library path." CACHE PATH "path to zmq libraries.")
    MESSAGE(FATAL_ERROR "find_zmq.cmake: unable to find zmq.")
  ENDIF ( NOT ZMQ_INCLUDE_DIRS AND NOT ZMQ_LIBRARY_DIRS )

  IF ( NOT ZMQ_INCLUDE_DIRS )
    SET(ZMQ_INCLUDE_SEARCH_DIR "Please provide zmq include path." CACHE PATH "path to zmq headers.")
    MESSAGE(FATAL_ERROR "find_zmq.cmake: unable to find zmq headers.")
  ELSE ( NOT ZMQ_INCLUDE_DIRS )
    UNSET(ZMQ_INCLUDE_SEARCH_DIR CACHE)
  ENDIF ( NOT ZMQ_INCLUDE_DIRS )

  IF ( NOT ZMQ_LIBRARY_DIRS )
    SET(ZMQ_LIBRARY_SEARCH_DIR "Please provide zmq library path." CACHE PATH "path to zmq libraries.")
    MESSAGE(FATAL_ERROR "find_zmq.cmake: unable to find zmq libraries.")
  ELSE ( NOT ZMQ_LIBRARY_DIRS )
    UNSET(ZMQ_LIBRARY_SEARCH_DIR CACHE)
  ENDIF ( NOT ZMQ_LIBRARY_DIRS ) 

ENDMACRO (request_zmq_search_directories)

##############################################################################
# search
##############################################################################
message(STATUS "-- checking for ZMQ")

IF (NOT ZMQ_INCLUDE_DIRS)

  SET(_ZMQ_FOUND_INC_DIRS "")
  FOREACH(_SEARCH_DIR ${ZMQ_INCLUDE_SEARCH_DIRS})
    FIND_PATH(_CUR_SEARCH
      NAMES zmq.h
        PATHS ${_SEARCH_DIR}
        NO_DEFAULT_PATH)
    IF (_CUR_SEARCH)
      LIST(APPEND _ZMQ_FOUND_INC_DIRS ${_CUR_SEARCH})
    ENDIF(_CUR_SEARCH)
    SET(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")
  ENDFOREACH(_SEARCH_DIR ${ZMQ_INCLUDE_SEARCH_DIRS})

  IF (NOT _ZMQ_FOUND_INC_DIRS)
    request_zmq_search_directories()
  ENDIF (NOT _ZMQ_FOUND_INC_DIRS)

  FOREACH(_INC_DIR ${_ZMQ_FOUND_INC_DIRS})
    SET(ZMQ_INCLUDE_DIRS ${ZMQ_INCLUDE_DIRS} ${_INC_DIR} CACHE PATH "zmq include directory.")
  ENDFOREACH(_INC_DIR ${_ZMQ_FOUND_INC_DIRS})

ENDIF (NOT ZMQ_INCLUDE_DIRS)

IF(UNIX)
  SET(ZMQ_LIB_FILENAME "libzmq.so")
ELSEIF(WIN32)
  SET(ZMQ_LIB_FILENAME "libzmq.lib")
ENDIF(UNIX)

IF ( NOT ZMQ_LIBRARY_DIRS )

  SET(_ZMQ_FOUND_LIB_DIR "")
  SET(_ZMQ_POSTFIX "")

  FOREACH(_SEARCH_DIR ${ZMQ_LIBRARY_SEARCH_DIRS})
    FIND_PATH(_CUR_SEARCH
      NAMES ${ZMQ_LIB_FILENAME}
        PATHS ${_SEARCH_DIR}
        PATH_SUFFIXES release debug
        NO_DEFAULT_PATH)
    IF (_CUR_SEARCH)
      LIST(APPEND _ZMQ_FOUND_LIB_DIR ${_SEARCH_DIR})
    ENDIF(_CUR_SEARCH)
    SET(_CUR_SEARCH _CUR_SEARCH-NOTFOUND CACHE INTERNAL "internal use")
  ENDFOREACH(_SEARCH_DIR ${ZMQ_LIBRARY_SEARCH_DIRS})

  IF (NOT _ZMQ_FOUND_LIB_DIR)
    request_zmq_search_directories()
  ELSE (NOT _ZMQ_FOUND_LIB_DIR)
    SET(ZMQ_LIBRARY_DIRS ${_ZMQ_FOUND_LIB_DIR} CACHE PATH "The zmq library directory")
  ENDIF (NOT _ZMQ_FOUND_LIB_DIR)

  FOREACH(_LIB_DIR ${_ZMQ_FOUND_LIB_DIR})
    LIST(APPEND _ZMQ_LIBRARIES ${ZMQ_LIB_FILENAME})
  ENDFOREACH(_LIB_DIR ${_ZMQ_FOUND_INC_DIRS})

  IF (_ZMQ_FOUND_LIB_DIR)
    SET(ZMQ_LIBRARIES ${_ZMQ_LIBRARIES} CACHE FILEPATH "The zmq library filename.")
  ENDIF (_ZMQ_FOUND_LIB_DIR)

ENDIF ( NOT ZMQ_LIBRARY_DIRS )

##############################################################################
# verify
##############################################################################
IF ( NOT ZMQ_INCLUDE_DIRS OR NOT ZMQ_LIBRARY_DIRS )
  request_zmq_search_directories()
ELSE ( NOT ZMQ_INCLUDE_DIRS OR NOT ZMQ_LIBRARY_DIRS ) 
  UNSET(ZMQ_INCLUDE_SEARCH_DIR CACHE)
  UNSET(ZMQ_LIBRARY_SEARCH_DIR CACHE)
  MESSAGE(STATUS "--  found matching zmq version")
ENDIF ( NOT ZMQ_INCLUDE_DIRS OR NOT ZMQ_LIBRARY_DIRS )
