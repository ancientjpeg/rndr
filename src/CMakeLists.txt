

add_subdirectory(rndr)

# see https://github.com/eliemichel/glfw3webgpu

install(CODE "MESSAGE(\"Installing shaders\")")
install(DIRECTORY shaders DESTINATION ${RNDR_SUPPORT_DIR})
