
add_subdirectory(rndr)
add_executable(rndr_test_program ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(rndr_test_program PUBLIC librndr)

# see https://github.com/eliemichel/glfw3webgpu

set(RNDR_SUPPORT_DIR /usr/local/share/rndr)
install(CODE "MESSAGE(\"Installing shaders\")")
install(DIRECTORY shaders DESTINATION ${RNDR_SUPPORT_DIR})
