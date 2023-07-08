
add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(jkwgpu)

# see https://github.com/eliemichel/glfw3webgpu
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui glfw3webgpu)

get_target_property(MY_PROJECT_SOURCES ${CMAKE_PROJECT_NAME} SOURCES)

message("LISTING SOURCES")
message(${MY_PROJECT_SOURCES})
