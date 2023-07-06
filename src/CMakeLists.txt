add_executable(App main.cpp wgpu_init.cpp)

# see https://github.com/eliemichel/glfw3webgpu

target_link_libraries(App PRIVATE webgpu glfw glfw3webgpu)
