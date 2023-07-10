
add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(jkwgpu)

# see https://github.com/eliemichel/glfw3webgpu
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui glfw3webgpu webgpu_cpp)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
get_target_property(MY_PROJECT_SOURCES ${CMAKE_PROJECT_NAME} SOURCES)

message("LISTING SOURCES")
message(${MY_PROJECT_SOURCES})
