cmake_minimum_required(VERSION 3.15)

project(rndr 
        VERSION 0.0.1 
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)



add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(tests)
