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

  WGPUQueue queue = wgpuDeviceGetQueue(globals.device);
  wgpuQueueOnSubmittedWorkDone(queue, 0, on_queue_finish, nullptr);

  WGPUSwapChainDescriptor swap_chain_desc = {};
  swap_chain_desc.nextInChain             = nullptr;
  swap_chain_desc.width                   = w;
  swap_chain_desc.height                  = h;
  swap_chain_desc.format                  = WGPUTextureFormat_BGRA8Unorm;
  swap_chain_desc.usage                   = WGPUTextureUsage_RenderAttachment;
  swap_chain_desc.presentMode             = WGPUPresentMode_Fifo;
  WGPUSwapChain swap_chain
      = wgpuDeviceCreateSwapChain(device, surface, &swap_chain_desc);

  WGPURenderPipeline pipeline = getWGPURenderPipeline(globals);

  while (!glfwWindowShouldClose(window)) {

    // Check whether the user clicked on the close button (and any other
    // mouse/key event, which we don't use so far)
    glfwPollEvents();

    /* get current texture to write to */
    WGPUTextureView next_tex = wgpuSwapChainGetCurrentTextureView(swap_chain);

    if (!next_tex) {
      std::cerr << "Cannot acquire next swap chain texture" << std::endl;
      break;
    }

    /* Command Encoder/Buffer Setup */
    WGPUCommandEncoderDescriptor encoder_desc = {};
    encoder_desc.nextInChain                  = nullptr;
    encoder_desc.label                        = "Default Command Encoder";
    WGPUCommandEncoder encoder
        = wgpuDeviceCreateCommandEncoder(globals.device, &encoder_desc);

    WGPUCommandBufferDescriptor command_buffer_desc = {};
    command_buffer_desc.nextInChain                 = nullptr;
    command_buffer_desc.label                       = "Default Command buffer";

    /* describe render pass color attachment */
    WGPURenderPassColorAttachment color_attachment = {};
    color_attachment.nextInChain                   = nullptr;
    color_attachment.view                          = next_tex;
    color_attachment.loadOp                        = WGPULoadOp_Clear;
    color_attachment.storeOp                       = WGPUStoreOp_Store;
    color_attachment.clearValue                    = {1, 0, 0, 1};

    /* describe render pass */
    WGPURenderPassDescriptor pass_desc = {};
    pass_desc.label                    = "Default Render Pass Encoder";
    pass_desc.colorAttachmentCount     = 1;
    pass_desc.colorAttachments         = &color_attachment;
    pass_desc.timestampWriteCount      = 0;
    pass_desc.timestampWrites          = nullptr;
    pass_desc.depthStencilAttachment   = nullptr;

    /* create render pass encoder */
    WGPURenderPassEncoder pass
        = wgpuCommandEncoderBeginRenderPass(encoder, &pass_desc);
    wgpuRenderPassEncoderSetPipeline(pass, pipeline);

    /* push events to render pass encoder */
    wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);

    /* finish encoding render pass */
    wgpuRenderPassEncoderEnd(pass);

    /* encode the command buffer, push to queue */
    WGPUCommandBuffer command_buffer
        = wgpuCommandEncoderFinish(encoder, &command_buffer_desc);
    wgpuQueueSubmit(queue, 1, &command_buffer);

    wgpuCommandEncoderRelease(encoder);
    wgpuCommandBufferRelease(command_buffer);

    wgpuTextureViewRelease(next_tex);

    wgpuSwapChainPresent(swap_chain);
  }

  wgpuRenderPipelineRelease(pipeline);
  wgpuSwapChainRelease(swap_chain);
  wgpuQueueRelease(queue);
  wgpuGlobalsRelease(globals);
  glfwDestroyWindow(window);
  glfwTerminate();
}
