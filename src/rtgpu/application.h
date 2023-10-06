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

#ifndef RTGPU_APPLICATION_H_
#define RTGPU_APPLICATION_H_

#include "webgpu/webgpu_cpp.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <map>

#ifndef RTGPU_SUPPORT_DIR
#error "Must define rtgpu support directory."
#endif

namespace rtgpu {

class Application {
public:
  Application();
  ~Application() = default;

  /* non-copyable */
  Application(const Application &)            = delete;
  Application &operator=(const Application &) = delete;

  /* non-movable */
  Application(Application &&)            = delete;
  Application &operator=(Application &&) = delete;

  void setRequiredFeatures(std::vector<wgpu::FeatureName> required_features);
  void setRequiredLimits(wgpu::Limits required_limits);

  /**
   * @brief Initialize all global WebGPU and GLFW objects.
   *
   * @note `initialize()` can throw.
   *
   * @param width Initial width of the screen, in pixels
   * @param height Initial height of the screen, in pixels
   */
  void initialize(int width = 640, int height = 480);

private:
  void collectShaderSource_(bool rescan = false);

  /* Global WGPU objects */
  wgpu::Instance                 instance_;
  wgpu::Surface                  surface_;
  wgpu::Adapter                  adapter_;
  wgpu::Device                   device_;

  std::filesystem::path          support_dir_       = {};
  std::filesystem::path          shader_dir_        = {};

  std::vector<wgpu::FeatureName> required_features_ = {};
  wgpu::RequiredLimits           required_limits_   = {};

  /* Global GLFW objects */
  GLFWwindow *window_ = {};

  /* Shader storage */
  std::map<std::string, std::string> shader_code_ = {};

  int                                width_       = 0;
  int                                height_      = 0;
};

} // namespace rtgpu

#endif