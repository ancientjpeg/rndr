/**
 * @file helpers.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "webgpu/webgpu_cpp.h"
#include <GLFW/glfw3.h>

#ifndef WGPU_HELPERS_H
#define WGPU_HELPERS_H

namespace rtgpu {

struct Globals {
  wgpu::Instance instance;
  wgpu::Adapter  adapter;
  wgpu::Device   device;
  wgpu::Surface  surface;
};

Globals              requestGlobals(GLFWwindow *window);

wgpu::RenderPipeline createRenderPipeline(Globals globals);

namespace helpers {

void default_device_lost_callback(WGPUDeviceLostReason reason,
                                  char const          *message,
                                  void                *userdata);

void default_error_callback(WGPUErrorType type,
                            char const   *message,
                            void         *userdata);
} // namespace helpers

} // namespace rtgpu

#endif