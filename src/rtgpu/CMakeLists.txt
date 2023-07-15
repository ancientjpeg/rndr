set(WGPU_SRC    
    ${CMAKE_CURRENT_SOURCE_DIR}/application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/matrix.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/ops.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/ops.cpp 
    )

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${WGPU_SRC})