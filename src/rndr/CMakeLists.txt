add_library(librndr STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
)

add_subdirectory(math)
add_subdirectory(resources)
add_subdirectory(utils)


target_include_directories(librndr PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(librndr PUBLIC imgui glfw3webgpu webgpu_cpp)

target_compile_definitions(librndr PUBLIC RNDR_SUPPORT_DIR=\"${RNDR_SUPPORT_DIR}\")