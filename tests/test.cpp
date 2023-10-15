#include "utils/obj_parser.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>

TEST_CASE("test OBJ parser")
{
  using namespace std::filesystem;
  std::string   test_obj = R"(
v 1.0 0.0 1.0
v 0.0 0.0 1.0
v 1.0 1.0 1.0
v 1.0 1.0 1.0
v 1.0 1.0 1.0

f 1 2 3
f 
)";

  path          p        = temp_directory_path() / "objtmp.obj";
  std::ofstream ofs(p);
  ofs << test_obj;
  ofs.close();

  auto obj = rndr::parseObjFile(p);
  REQUIRE(obj.has_value());
}