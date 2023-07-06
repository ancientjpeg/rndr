add_executable(App main.cpp wgpu_init.h)

target_include_directories(App PRIVATE ../build/_deps/imgui-src .)
target_link_libraries(App PRIVATE webgpu glfw)