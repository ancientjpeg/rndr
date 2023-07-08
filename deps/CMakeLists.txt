include(FetchContent)


FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution
  GIT_TAG        main # see repo README to see how to use different branches
)
FetchContent_MakeAvailable(webgpu)





FetchContent_Declare(
    glfw3webgpu
    GIT_REPOSITORY https://github.com/eliemichel/glfw3webgpu
    GIT_TAG main
)
FetchContent_MakeAvailable(glfw3webgpu)

# # https://jonathanhamberg.com/post/2019-01-27-cmake-dependencies-fetch-content/
# FetchContent_Declare(
#         glfw
#         GIT_REPOSITORY https://github.com/glfw/glfw
# )

# FetchContent_GetProperties(glfw)
# if(NOT glfw_POPULATED)
#     FetchContent_Populate(glfw)

#     set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
#     set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
#     set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
#     set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

#     add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
# endif()