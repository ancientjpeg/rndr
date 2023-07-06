cmake_minimum_required(VERSION 3.0)

project(LearnWebGPU 
        VERSION 0.0.1 
        LANGUAGES C CXX)

# set(WEBGPU_BACKEND "DAWN" CACHE STRING "Backend Type")



add_subdirectory(deps)
add_subdirectory(src)
