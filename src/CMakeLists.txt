
set(EXEC_FILES main.cpp wgpu_init.cpp)
add_executable(WebGPUTester ${EXEC_FILES})

# see https://github.com/eliemichel/glfw3webgpu

target_link_libraries(WebGPUTester PRIVATE imgui glfw3webgpu)
