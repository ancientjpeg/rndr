#include "wgpu_init.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#include <imgui.h>
#include <iostream>

static WGPUGlobals globals;
constexpr int      w = 640;
constexpr int      h = 480;

int                main()
{

  glfwInitHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  glfwInit();
  GLFWwindow *window = glfwCreateWindow(w, h, "LearnWebGPU", nullptr, nullptr);

  if (!window) {
    std::cerr << "GLFW could not open window!" << std::endl;
    glfwTerminate();
    return 1;
  }

  globals     = requestWGPUGlobals();
  auto device = globals.device;

  /* include this as compatible surface for the adapter in the future */
  auto surface         = glfwGetWGPUSurface(globals.instance, window);

  auto on_queue_finish = [](WGPUQueueWorkDoneStatus status, void *) {
    std::cout << "FINISHED QUEUE WORK" << std::endl;
  };

  WGPUSwapChainDescriptor swap_chain_desc = {};
  swap_chain_desc.nextInChain             = nullptr;
  swap_chain_desc.width                   = w;
  swap_chain_desc.height                  = h;
  swap_chain_desc.format                  = WGPUTextureFormat_RGBA8Unorm;
  swap_chain_desc.usage                   = WGPUTextureUsage_RenderAttachment;
  swap_chain_desc.presentMode             = WGPUPresentMode_Fifo;
  WGPUSwapChain swap_chain
      = wgpuDeviceCreateSwapChain(device, surface, &swap_chain_desc);

  WGPUQueue queue = wgpuDeviceGetQueue(globals.device);
  wgpuQueueOnSubmittedWorkDone(queue, 0, on_queue_finish, nullptr);

  WGPUCommandEncoderDescriptor encoder_desc = {};
  encoder_desc.nextInChain                  = nullptr;
  encoder_desc.label                        = "WGPU Default Command Encoder";
  WGPUCommandEncoder encoder
      = wgpuDeviceCreateCommandEncoder(globals.device, &encoder_desc);
  wgpuCommandEncoderInsertDebugMarker(encoder, "BUPPUS");

  WGPUCommandBufferDescriptor command_buffer_desc = {};
  command_buffer_desc.nextInChain                 = nullptr;
  command_buffer_desc.label                       = "Command buffer";
  WGPUCommandBuffer command_buffer
      = wgpuCommandEncoderFinish(encoder, &command_buffer_desc);

  wgpuQueueSubmit(queue, 1, &command_buffer);

  // wgpuCommandEncoderRelease(encoder);
  // wgpuCommandBufferRelease(command_buffer);

  while (!glfwWindowShouldClose(window)) {
    // Check whether the user clicked on the close button (and any other
    // mouse/key event, which we don't use so far)
    glfwPollEvents();

    WGPUTextureView next_tex = wgpuSwapChainGetCurrentTextureView(swap_chain);

    if (!next_tex) {
      std::cerr << "Cannot acquire next swap chain texture" << std::endl;
      break;
    }

    WGPURenderPassDescriptor pass_desc = {};
    WGPURenderPassEncoder    pass
        = wgpuCommandEncoderBeginRenderPass(encoder, &pass_desc);
    wgpuRenderPassEncoderEnd(pass);

    // wgpuRenderPassEncoderRelease(pass);
    wgpuTextureViewRelease(next_tex);
  }

  wgpuQueueRelease(queue);
  wgpuGlobalsRelease(globals);
  glfwDestroyWindow(window);
  glfwTerminate();
}
