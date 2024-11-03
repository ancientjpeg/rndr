#include "rndr/application.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("Application initializes and destructs without fault", "[sanity]")
{
  auto         app         = std::make_unique<rndr::Application>();
  rndr::Result init_result = app->initialize();
  INFO("Failed to initialize: " << init_result);
  REQUIRE(!init_result.ok());
  REQUIRE(app->isInitialized());
}
