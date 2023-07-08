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

#ifndef WGPU_HELPERS_H
#define WGPU_HELPERS_H

namespace jkwgpu {

struct Globals {
  WGPUInstance instance = nullptr;
  WGPUAdapter  adapter  = nullptr;
  WGPUDevice   device   = nullptr;
};

Globals            requestGlobals();

WGPURenderPipeline createRenderPipeline(Globals globals);

void               releaseGlobals(Globals globals);

} // namespace jkwgpu

#endif