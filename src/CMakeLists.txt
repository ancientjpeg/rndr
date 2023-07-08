add_executable(App main.cpp wgpu_init.cpp)

# see https://github.com/eliemichel/glfw3webgpu

target_include_directories(App PUBLIC ${FETCHCONTENT_BASE_DIR}/imgui-src )
target_link_libraries(App PRIVATE webgpu glfw glfw3webgpu)
