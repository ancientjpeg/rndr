#include "rndr/math/matrix.h"
#include <catch2/catch_test_macros.hpp>

using namespace rndr;

TEST_CASE("matrix indexing")
{
  math::matrix<2, 3> mat({0, 1, 2, 3, 4, 5});
  REQUIRE(mat.at(1, 1) == 4);
}

TEST_CASE("matrix addition")
{
  math::matrix<3, 2> mat0({1, 0, 1, 0, 1, 0});
  math::matrix<3, 2> mat1({0, 1, 0, 1, 0, 1});

  REQUIRE(mat0 + mat1 == decltype(mat0)({1, 1, 1, 1, 1, 1}));
}

TEST_CASE("matrix multiplication")
{

  math::matrix<2, 3> mat2({1, 0, 2, 2, 0, 2});
  math::vector<3>    mat3({1, 1, 0});
  math::vector<2>    vector_result = mat2 * mat3;
  REQUIRE(mat2 * mat2.identity<mat2.dim<1>()>() == mat2);
  REQUIRE(decltype(mat2)::identity<mat2.dim<0>()>() * mat2 == mat2);
  REQUIRE(vector_result == math::vector<2>({1, 2}));

  math::matrix<2, 3> matrix0({0, 1, 2, 3, 4, 5});
  math::matrix<3, 2> matrix1({1, 0, 1, 0, 1, 0});
  math::matrix<2, 2> result = matrix0 * matrix1;
  REQUIRE(result == math::matrix<2, 2>({3, 0, 12, 0}));
}