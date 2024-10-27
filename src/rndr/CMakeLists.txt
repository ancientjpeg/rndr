add_library(rndr STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
)

add_subdirectory(math)
add_subdirectory(resources)
add_subdirectory(types)
add_subdirectory(utils)


target_include_directories(rndr PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(rndr PUBLIC glfw3webgpu webgpu_cpp)

target_compile_definitions(rndr PUBLIC RNDR_SUPPORT_DIR=\"${RNDR_SUPPORT_DIR}\")
