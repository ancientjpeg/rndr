add_executable(example0 ${CMAKE_CURRENT_SOURCE_DIR}/example0.cpp)
target_link_libraries(example0 PUBLIC rndr)

add_executable(example1 ${CMAKE_CURRENT_SOURCE_DIR}/example1.cpp)
target_link_libraries(example1 PUBLIC rndr)
