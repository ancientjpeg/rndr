cmake_minimum_required(VERSION 3.0)

project(LearnWebGPU 
        VERSION 0.0.1 
        LANGUAGES C CXX)

# set(WEBGPU_BACKEND "DAWN" CACHE STRING "Backend Type")

include(FetchContent)
FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution
  GIT_TAG        main
)
FetchContent_MakeAvailable(webgpu)


add_subdirectory(src)
