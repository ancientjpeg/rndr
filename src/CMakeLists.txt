

add_subdirectory(rndr)

install(CODE "MESSAGE(\"Installing shaders\")")
install(DIRECTORY shaders DESTINATION ${RNDR_SUPPORT_DIR})
