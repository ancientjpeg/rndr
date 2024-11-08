/**
 * @file result.tests.cpp
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright © 2024 Minimal. All rights reserved.
 */

#include "rndr/utils/result.h"
#include <catch2/catch_test_macros.hpp>
#include <sstream>

TEST_CASE("Result Compiles And Behaves As Expected")
{
  std::stringstream ss;

  const std::string fail_msg = "Failed!";
  const std::string expected_ostream_fail_msg
      = "Failed with message: " + fail_msg;
  const std::string expected_ostream_empty_fail_msg = "Failed without message.";
  const std::string expected_ostream_success_msg    = "Succeeded";
  using namespace rndr;

  SECTION("Successful Results Give Expected Message")
  {
    Res double_res(5.);

    /* quickly check type inference is working properly */
    static_assert(std::is_same_v<double, decltype(double_res)::type>);

    ss << double_res;
    CHECK(ss.str() == expected_ostream_success_msg);
  }

  SECTION("Void Results Behave As Expected")
  {
    auto void_success = Result{};
    CHECK(void_success.ok());

    Result void_failure = Failure(fail_msg);
    CHECK(!void_failure.ok());
    CHECK(void_failure.message() == fail_msg);

    ss << void_failure;
    CHECK(ss.str() == expected_ostream_fail_msg);
  }

  SECTION("String Results Behave As Expected")
  {
    auto string_success = Res(std::string("What!"));
    CHECK(*string_success == "What!");

    decltype(string_success) string_failure = Failure("Failed!");
    CHECK_THROWS_AS(*string_failure, std::bad_variant_access);
    CHECK(string_failure.message() == fail_msg);

    ss << string_failure;
    CHECK(ss.str() == expected_ostream_fail_msg);
  }

  SECTION("Int Results Behave As Expected")
  {
    auto int_success = Res(5);
    CHECK(*int_success == 5);

    decltype(int_success) int_failure = Failure("Failed!");
    CHECK_THROWS_AS(*int_failure, std::bad_variant_access);
    CHECK(int_failure.message() == fail_msg);

    ss << int_failure;
    CHECK(ss.str() == expected_ostream_fail_msg);
  }
}
