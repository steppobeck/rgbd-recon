file(GLOB_RECURSE FRAMEWORK_SOURCE *.cpp *.c)
file(GLOB_RECURSE FRAMEWORK_INLINE *.inl)
file(GLOB_RECURSE FRAMEWORK_HEADER *.hpp)

source_group("Inline Files" FILES ${FRAMEWORK_INLINE})

add_library(framework STATIC
  ${FRAMEWORK_SOURCE}
  ${FRAMEWORK_INLINE}
  ${FRAMEWORK_HEADER})

# self built libraries
target_link_libraries(framework gloost fastdxt ${SQUISH_LIBRARIES} globjects glbinding)
# global libraries
target_link_libraries(framework GLU zmq CGAL ${BOOST_LIBRARIES} gmp mpfr -fopenmp)
set_target_properties(framework PROPERTIES COMPILE_FLAGS "-fopenmp")

target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/calibration)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/navigation)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/measurement)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/rendering)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/io)
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/reconstruction)