file(GLOB_RECURSE FRAMEWORK_SOURCE *.cpp *.c)
file(GLOB_RECURSE FRAMEWORK_INLINE *.inl)
file(GLOB_RECURSE FRAMEWORK_HEADER *.hpp)

source_group("Inline Files" FILES ${FRAMEWORK_INLINE})

add_library(framework STATIC
  ${FRAMEWORK_SOURCE}
  ${FRAMEWORK_INLINE}
  ${FRAMEWORK_HEADER})

target_link_libraries(framework ${SVG_LIBRARIES})
target_link_libraries(framework ${GLOOST_LIBRARIES})
target_link_libraries(framework ${FASTDXT_LIBRARIES})

target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/calibration)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/navigation)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/measurement)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rendering)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/io)