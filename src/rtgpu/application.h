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
#include <filesystem>
#include <iostream>

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

  void         Initialize();

private:
  /* Global WGPU objects */
  wgpu::Instance        instance_;
  wgpu::Surface         surface_;
  wgpu::Adapter         adapter_;
  wgpu::Device          device_;

  std::filesystem::path support_dir;
  std::filesystem::path shader_dir;

  /* Global GLFW objects */

  int width_  = 0;
  int height_ = 0;
};

} // namespace rtgpu

#endif