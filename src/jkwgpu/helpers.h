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

#include "glfw3webgpu.h"
#include "webgpu/webgpu_cpp.h"

#ifndef WGPU_HELPERS_H
#define WGPU_HELPERS_H

namespace jkwgpu {

struct Globals {
  wgpu::Instance instance = nullptr;
  wgpu::Adapter  adapter  = nullptr;
  wgpu::Device   device   = nullptr;
};

Globals              requestGlobals();

wgpu::RenderPipeline createRenderPipeline(Globals globals);

} // namespace jkwgpu

#endif