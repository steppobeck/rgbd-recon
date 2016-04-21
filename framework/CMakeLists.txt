file(GLOB_RECURSE FRAMEWORK_SOURCE *.cpp)
file(GLOB_RECURSE FRAMEWORK_INLINE *.inl)
file(GLOB_RECURSE FRAMEWORK_HEADER *.hpp)

source_group("Inline Files" FILES ${FRAMEWORK_INLINE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/calibration)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/navigation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/measurement)

add_library(framework STATIC glew.c
  ${FRAMEWORK_SOURCE}
  ${FRAMEWORK_INLINE}
  ${FRAMEWORK_HEADER})
