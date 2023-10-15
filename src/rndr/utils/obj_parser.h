/**
 * @file wavefront_parser.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_OBJ_PARSER_H_
#define RNDR_OBJ_PARSER_H_

#include "math/matrix.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

namespace rndr {

struct Vertex {
  std::vector<math::vec<3>>         vertices;
  std::vector<math::vec<3, size_t>> faces;
};

struct MeshData {};

static std::optional<MeshData> parseObjFile(std::filesystem::path obj_path)
{
  using namespace std::filesystem;
  if (!exists(obj_path) || obj_path.extension() != ".obj") {
    return std::nullopt;
  }

  std::ifstream ifs(obj_path);

  for (std::string line; std::getline(ifs, line);) {
    // TRIM WHITESPACE
    //
  }

  return std::nullopt;
}

} // namespace rndr

#endif