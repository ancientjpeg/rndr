include(FetchContent)


if ("${WEBGPU_BACKEND}" STREQUAL "DAWN")

# why does dawn just shove glfw directly into my anus like this
set(DAWN_BUILD_SAMPLES OFF)
set(DAWN_USE_GLFW OFF)

else()

message(FATAL_ERROR "if you dont use dawn im kill you")

endif()


FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution
  GIT_TAG        fb39c1fcbd3c48da8e87fe614194a217a6e04fae
  # GIT_TAG        main # see repo README to see how to use different branches
)
FetchContent_MakeAvailable(webgpu)


FetchContent_Declare(
    glfw3webgpu
    GIT_REPOSITORY https://github.com/eliemichel/glfw3webgpu
    GIT_TAG 39a80205998c6cbf803e18e750d964c23a47ef39
    # GIT_TAG main
)
FetchContent_MakeAvailable(glfw3webgpu)

# https://jonathanhamberg.com/post/2019-01-27-cmake-dependencies-fetch-content/
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 2b3f919b6055e6837ca0fad193c7f96b323f1256
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

### IMGUI SETUP ###

set(IMGUI_RENDERER "WebGPU")
set(IMGUI_PLATFORM "GLFW")

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set(IMGUI_SRC "${IMGUI_DIR}/imgui.cpp")

set(IMGUI_INCLUDE_DIRS ${IMGUI_DIR})

if (${IMGUI_RENDERER} STREQUAL "WebGPU")
    set(IMGUI_INCLUDE_DIRS ${IMGUI_INCLUDE_DIRS} ${FETCHCONTENT_BASE_DIR}/dawn_src/include)
    # set(IMGUI_SRC ${IMGUI_SRC} ${IMGUI_DIR}/backends/imgui_impl_wgpu.cpp)
endif()

if (${IMGUI_PLATFORM} STREQUAL "GLFW")
    set(IMGUI_INCLUDE_DIRS ${IMGUI_INCLUDE_DIRS} ${FETCHCONTENT_BASE_DIR}/glfw_src/include)
    # set(IMGUI_SRC ${IMGUI_SRC} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp)
endif()


add_library(imgui ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIRS})
target_link_libraries(imgui glfw webgpu)
