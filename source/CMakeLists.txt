add_executable(test_stuff test_stuff.cpp)
target_link_libraries(test_stuff framework ${LIBRARIES})
add_dependencies(test_stuff glfw framework ${COPY_BINARY})

