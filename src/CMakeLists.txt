
file(GLOB IMGUI_SRC ../deps/imgui/*.cpp)
set(EXEC_FILES main.cpp wgpu_init.cpp ${IMGUI_SRC})
add_executable(App ${EXEC_FILES})

# see https://github.com/eliemichel/glfw3webgpu

target_include_directories(App PUBLIC ../deps/imgui)
target_link_libraries(App PRIVATE webgpu glfw glfw3webgpu)
