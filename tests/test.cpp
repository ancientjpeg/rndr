#include "math/matrix.h"
#include "utils/obj_parser.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>

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

TEST_CASE("test OBJ parser")
{
  using namespace std::filesystem;

  rndr::MeshData md
      = {{math::vector<3>({1, 0, 1}), math::vector<3>({0, 0, 1}),
          math::vector<3>({1, 1, 1}), math::vector<3>({1, 1, 0})},
         {math::svector<3>({1, 2, 3}), math::svector<3>({1, 2, 4}),
          math::svector<3>({2, 3, 4}), math::svector<3>({1, 3, 4})}};

  std::string   test_obj = R"(
  v 1.0 0.0 1.0
  v 0.0 0.0 1.0
v 1.0 1.0 1.0
    v 1.0 1.0 0.0

  f 1 2 3
      f 1 2 4
  f 2 3 4
    f 1 3 4
  )";

  path          p        = temp_directory_path() / "objtmp.obj";
  std::ofstream ofs(p);
  ofs << test_obj;
  ofs.close();

  auto obj = rndr::parseObjFile(p);
  REQUIRE(obj.has_value());
  REQUIRE(*obj == md);
}