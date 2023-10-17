#include "math/matrix.h"
#include "utils/obj_parser.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>

using namespace rndr;
TEST_CASE("test matrices")
{
  math::matrix<3, 2> mat0({1, 0, 1, 0, 1, 0});
  math::matrix<3, 2> mat1({0, 1, 0, 1, 0, 1});

  REQUIRE(mat0 + mat1 == decltype(mat0)({1, 1, 1, 1, 1, 1}));
  math::matrix<2, 3> TEST({0, 1, 2, 3, 4, 5});
  REQUIRE(TEST.at(1UL, 1UL) == 4);
  REQUIRE(TEST.at(1, 1) == 4);

  math::matrix<2, 3> mat2({1, 0, 2, 1, 0, 2});
  math::matrix<3, 1> mat3({1, 0, 0});
  auto               res = mat2 * mat3;
  REQUIRE(mat0 * mat0.identity<mat0.dim<1>()>() == mat0);
}

TEST_CASE("test OBJ parser")
{
  using namespace std::filesystem;

  //   rndr::MeshData md       = {{{1, 0, 1}, {0, 0, 1}, {1, 1, 1}, {1, 1, 0}},
  //                              {{1, 2, 3}, {1, 2, 4}, {2, 3, 4}, {1, 3, 4}}};

  //   std::string    test_obj = R"(
  // v 1.0 0.0 1.0
  // v 0.0 0.0 1.0
  // v 1.0 1.0 1.0
  // v 1.0 1.0 0.0

  // f 1 2 3
  //     f 1 2 4
  // f 2 3 4
  //   f 1 3 4
  // )";

  //   path           p        = temp_directory_path() / "objtmp.obj";
  //   std::ofstream  ofs(p);
  //   ofs << test_obj;
  //   ofs.close();

  //   auto obj = rndr::parseObjFile(p);
  //   REQUIRE(obj.has_value());
  //   REQUIRE(*obj == md);
}