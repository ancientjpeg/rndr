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
#include <GLFW/glfw3.h>
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
  Application(Application &&)            = delete;
  Application &operator=(Application &&) = delete;

  /**
   * @brief Set the required features. Call this before a call to `initialize`.
   */
  void setRequiredFeatures(std::vector<wgpu::FeatureName> required_features);

  /**
   * @brief Set the required limits. Call this before a call to `initialize`.
   */
  void setRequiredLimits(wgpu::Limits required_limits);

  /**
   * @brief Initialize all global WebGPU and GLFW objects.
   *
   * @note `initialize()` can throw.
   *
   * @param width Initial width of the screen, in pixels
   * @param height Initial height of the screen, in pixels
   */
  void                initialize(int width = 640, int height = 480);

  bool                addShaderSource(std::filesystem::path shader_path);

  wgpu::TextureFormat get_preferred_texture_format();

private:
  void collectShaderSource_(bool rescan = false);

  /* Global WGPU objects */
  wgpu::Surface   surface_;

  wgpu::SwapChain swap_chain_;

  /* Features and limits */
  std::vector<wgpu::FeatureName> required_features_ = {};
  wgpu::RequiredLimits           required_limits_   = {};

  /* Global GLFW objects */
  GLFWwindow *window_ = {};

  /* Shader storage */
  struct ShaderSource {
    std::string           name;
    std::filesystem::path path;
    std::string           source;
  };
  std::map<std::string, ShaderSource> shader_code_ = {};

  std::filesystem::path               support_dir_ = {};
  std::filesystem::path               shader_dir_  = {};

  int                                 width_       = 0;
  int                                 height_      = 0;

  std::mutex                          frame_lock;
};

} // namespace rndr

#endif