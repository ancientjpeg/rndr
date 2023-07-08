#include "glfw3webgpu.h"
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>

#ifndef WGPU_INIT_H
#define WGPU_INIT_H

struct WGPUGlobals {
  WGPUInstance instance = nullptr;
  WGPUAdapter  adapter  = nullptr;
  WGPUDevice   device   = nullptr;
};

WGPUGlobals        requestWGPUGlobals();

WGPURenderPipeline getWGPURenderPipeline(WGPUGlobals globals);

void               wgpuGlobalsRelease(WGPUGlobals globals);

#endif