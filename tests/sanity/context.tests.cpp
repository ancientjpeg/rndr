#include "rndr/context.h"
#include "ustd/expected.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

/** TODO unhide this test when we are able to initialize without a surface */
TEST_CASE("Application initializes and destructs without fault", "[.sanity]")
{
  auto           context     = std::make_unique<rndr::Globals>();
  ustd::expected init_result = context->initialize();
  INFO("Failed to initialize: " << init_result);
  REQUIRE(!init_result.ok());
  REQUIRE(context->isInitialized());
}
