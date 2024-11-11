# Include tests that cannot run on GH actions due to lack of GPU here
if(NOT DEFINED ENV{ACTIONS_UNIT_TESTS}) 

target_sources(tests PRIVATE
  context.tests.cpp
)

endif()
