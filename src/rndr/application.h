/**
 * @file application.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_APPLICATION_H_
#define RNDR_APPLICATION_H_

#include "rndr/types/globals.h"

#include "webgpu/webgpu_cpp.h"
#include <filesystem>
#include <map>

#ifndef RNDR_SUPPORT_DIR
#error "Must define rndr support directory."
#endif

namespace rndr {

class Application : public Globals {
public:
  Application();
  ~Application() = default;

  /* non-copyable */
  Application(const Application &)            = delete;
  Application &operator=(const Application &) = delete;

  /* non-movable */
  Application(Application &&)                   = delete;
  Application        &operator=(Application &&) = delete;

  bool                addShaderSource(std::filesystem::path shader_path);

  wgpu::TextureFormat get_preferred_texture_format();

private:
  void collectShaderSource_(bool rescan = false);

  /* Shader storage */
  struct ShaderSource {
    std::string           name;
    std::filesystem::path path;
    std::string           source;
  };
  std::map<std::string, ShaderSource> shader_code_ = {};

  std::filesystem::path               support_dir_ = {};
  std::filesystem::path               shader_dir_  = {};

  std::mutex                          frame_lock;
};

} // namespace rndr

#endif
