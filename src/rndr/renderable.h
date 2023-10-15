/**
 * @file renderable.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_RENDERABLE_H_
#define RNDR_RENDERABLE_H_

#include <filesystem>
#include <optional>

namespace rndr {
class RenderableObject {
public:
  static std::optional<RenderableObject> fromFile(std::filesystem::path path);

private:
};
} // namespace rndr

#endif