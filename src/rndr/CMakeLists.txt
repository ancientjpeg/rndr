set(WGPU_SRC    
    ${CMAKE_CURRENT_SOURCE_DIR}/application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/renderable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/helpers.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/helpers.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/math/math.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/math/ops.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/math/ops.cpp 
    )

add_library(librndr STATIC)
target_sources(librndr PRIVATE ${WGPU_SRC})
target_include_directories(librndr PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(librndr PUBLIC imgui glfw3webgpu webgpu_cpp)

target_compile_definitions(librndr PUBLIC RNDR_SUPPORT_DIR=\"${RNDR_SUPPORT_DIR}\")