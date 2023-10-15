/**
 * @file renderable.cpp
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "renderable.h"
#include <fstream>

namespace rndr {

std::optional<RenderableObject>
RenderableObject::fromFile(std::filesystem::path path)
{
  using namespace std::filesystem;

  std::ifstream ifs(path);

  return std::nullopt;
}

} // namespace rndr