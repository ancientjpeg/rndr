
add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(rtgpu)

# see https://github.com/eliemichel/glfw3webgpu
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui glfw3webgpu webgpu_cpp)

get_target_property(MY_PROJECT_SOURCES ${CMAKE_PROJECT_NAME} SOURCES)

# message("LISTING SOURCES")
# message(${MY_PROJECT_SOURCES})

set(RTGPU_SUPPORT_DIR /usr/local/share/rtgpu)
install(CODE "MESSAGE(\"Installing shaders\")")
install(DIRECTORY shaders DESTINATION ${RTGPU_SUPPORT_DIR})
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RTGPU_SUPPORT_DIR=\"${RTGPU_SUPPORT_DIR}\")
