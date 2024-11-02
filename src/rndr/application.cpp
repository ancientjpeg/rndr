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
#include "rndr/utils/helpers.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

namespace rndr {

using namespace wgpu;

Application::Application()
    : support_dir_(RNDR_SUPPORT_DIR), shader_dir_(support_dir_ / "shaders")
{
  /** @todo DELETE! */
  /*
  assert(support_dir_.is_absolute() && std::filesystem::exists(support_dir_));
  assert(shader_dir_.is_absolute() && std::filesystem::exists(shader_dir_));

  collectShaderSource_(true);
  */
}

bool Application::addShaderSource(std::filesystem::path shader_path)
{
  using namespace std::filesystem;

  if (!exists(shader_path) || shader_path.extension() != ".wgsl") {
    return false;
  }

  std::stringstream out;
  std::ifstream     ifs(shader_path);
  out << ifs.rdbuf();

  auto name = shader_path.string().substr(shader_dir_.string().size() + 1);
  shader_code_[name] = {name, shader_path, out.str()};
  return true;
}

void Application::collectShaderSource_(bool rescan)
{

  using namespace std::filesystem;

  if (rescan) {
    shader_code_.clear();
  };

  for (auto const &dir_entry : recursive_directory_iterator{shader_dir_}) {
    auto p = dir_entry.path();
    addShaderSource(p);
  }
}

} // namespace rndr
