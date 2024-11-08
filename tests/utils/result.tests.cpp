/**
 * @file result.tests.cpp
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright © 2024 Minimal. All rights reserved.
 */

#include "rndr/utils/result.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Result Compiles And Behaves As Expected")
{
  using namespace rndr;
  auto void_success = Result{};
  CHECK(void_success.ok());

  Result void_failure = Failure("Failed!");
  CHECK(!void_success.ok());
  CHECK(void_failure.message() == "Failed with message: Failed!");

  /* auto void_success = Errc(); */
  auto string_success = Res(std::string("What!"));
  CHECK(*string_success == "What!");
}
