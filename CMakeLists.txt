cmake_minimum_required(VERSION 3.15)

project(rndr 
        VERSION 0.0.1 
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)


set(RNDR_SUPPORT_DIR /usr/local/share/rndr)

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(example)

if(DEFINED RNDR_BUILD_EXAMPLES)
endif()
