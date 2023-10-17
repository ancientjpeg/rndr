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
#include <map>
#include <optional>
#include <regex>
#include <vector>

namespace rndr {

struct MeshData {
  std::vector<math::vec3>  vertices;
  std::vector<math::svec3> faces;
};

static bool operator==(MeshData md0, MeshData md1)
{
  return md0.vertices == md1.vertices && md0.faces == md1.faces;
}

static std::optional<MeshData> parseObjFile(std::filesystem::path obj_path)
{

  enum class RowType { Vertex, Face, Invalid };
  const std::map<char, RowType> row_type_map
      = {{{'v', RowType::Vertex}, {'f', RowType::Face}}};

  if (!std::filesystem::exists(obj_path) || obj_path.extension() != ".obj") {
    return std::nullopt;
  }

  std::ifstream ifs(obj_path);

  std::regex    word_matcher(R"([\w\./]+)");
  std::smatch   matches;

  MeshData      ret;

  for (std::string line; std::getline(ifs, line);) {

    RowType                   row_type = RowType::Invalid;
    math::vec3                vertex;
    math::svec3               face;
    decltype(MeshData::faces) poly;
    for (size_t i = 0; std::regex_search(line, matches, word_matcher); ++i) {
      auto token = matches[0].str();

      if (i == 0) {
        if (token[0] == '#') {
          break;
        }
        /* This will throw if we hit a line where the first char */
        try {
          row_type = row_type_map.at(token.c_str()[0]);
        }
        catch (std::out_of_range &e) {
          /* unsupported dtype, continue. */
          break;
        }
      }
      else {
        switch (row_type) {
        case RowType::Vertex:
          vertex.at(i - 1) = std::stof(token);
          break;
        case RowType::Face:
          face.at(i - 1) = std::stoul(token);
          break;
        case RowType::Invalid:
          break;
        }
      }

      line = matches.suffix();
    }

    switch (row_type) {
    case RowType::Vertex:
      ret.vertices.push_back(std::move(vertex));
      break;
    case RowType::Face:
      ret.faces.push_back(std::move(face));
      break;
    case RowType::Invalid:
      break;
    }
  }

  if (ret.faces.empty() || ret.vertices.empty()) {
    return std::nullopt;
  }

  return ret;
}

} // namespace rndr

#endif