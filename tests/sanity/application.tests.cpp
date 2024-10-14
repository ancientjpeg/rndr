#include "rndr/application.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("Application initializes and destructs without fault")
{
  auto app = std::make_unique<rndr::Application>();
  app->initialize();
  app = nullptr;
}
