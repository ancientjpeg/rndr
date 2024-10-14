#include "rndr/math/matrix.h"
#include "rndr/utils/obj_parser.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>

using namespace rndr;

TEST_CASE("test OBJ parser")
{
  using namespace std::filesystem;

  rndr::MeshData md
      = {{math::vector<3>({1, 0, 1}), math::vector<3>({0, 0, 1}),
          math::vector<3>({1, 1, 1}), math::vector<3>({1, 1, 0})},
         {math::zvector<3>({1, 2, 3}), math::zvector<3>({1, 2, 4}),
          math::zvector<3>({2, 3, 4}), math::zvector<3>({1, 3, 4})}};

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