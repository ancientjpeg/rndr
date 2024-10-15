#include "rndr/application.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("Application initializes and destructs without fault")
{
  auto         app         = std::make_unique<rndr::Application>();
  rndr::Result init_result = app->initialize();
  CHECK(init_result.ok());
  CHECK(app->isInitialized());
  app = nullptr;
}
