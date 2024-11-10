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

#include "rndr/context.h"

#include "webgpu/webgpu_cpp.h"
#include <GLFW/glfw3.h>

#ifndef WGPU_HELPERS_H
#define WGPU_HELPERS_H

namespace rndr {

wgpu::RenderPipeline createRenderPipeline(Context &globals);

namespace helpers {

void default_device_lost_callback(WGPUDevice const    *device,
                                  WGPUDeviceLostReason reason,
                                  char const          *message,
                                  void                *userdata);

void default_error_callback(WGPUErrorType type,
                            char const   *message,
                            void         *userdata);

bool limits_supported(wgpu::Limits required_limits,
                      wgpu::Limits supported_limits);

} // namespace helpers

} // namespace rndr

#endif
