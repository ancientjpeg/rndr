/**
 * @file application.cpp
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "application.h"
#include <cassert>

namespace rtgpu {

Application::Application()
    : support_dir(RTGPU_SUPPORT_DIR), shader_dir(support_dir / "shaders")
{
  assert(support_dir.is_absolute() && std::filesystem::exists(support_dir));
  assert(shader_dir.is_absolute() && std::filesystem::exists(shader_dir));
  std::cout << support_dir << std::endl;
  std::cout << shader_dir << std::endl;
}

} // namespace rtgpu