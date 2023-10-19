add_library(librndr STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable.cpp
)
add_subdirectory(utils)
add_subdirectory(math)


target_include_directories(librndr PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(librndr PUBLIC imgui glfw3webgpu webgpu_cpp)

target_compile_definitions(librndr PUBLIC RNDR_SUPPORT_DIR=\"${RNDR_SUPPORT_DIR}\")