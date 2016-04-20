add_executable(test_stuff test_stuff.cpp)
target_link_libraries(test_stuff framework ${LIBRARIES})
add_dependencies(test_stuff glfw framework ${COPY_BINARY})

add_executable(kinect_client kinect_client.cpp)
target_link_libraries(kinect_client framework ${LIBRARIES})
add_dependencies(kinect_client glfw framework ${COPY_BINARY})

