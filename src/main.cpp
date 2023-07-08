#include "jkwgpu/helpers.h"
#include "jkwgpu/ops.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#include <imgui.h>
#include <iostream>

static jkwgpu::Globals globals;
constexpr int          w = 640;
constexpr int          h = 480;

int                    main()
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

  globals     = jkwgpu::requestGlobals();
  auto device = globals.device;

  /* include this as compatible surface for the adapter in the future */
  auto surface         = glfwGetWGPUSurface(globals.instance.Get(), window);

  auto on_queue_finish = [](WGPUQueueWorkDoneStatus status, void *) {
    std::cout << "FINISHED QUEUE WORK" << std::endl;
  };

  wgpu::Queue queue = globals.device.GetQueue();
  queue.OnSubmittedWorkDone(0, on_queue_finish, nullptr);
  wgpu::SwapChainDescriptor swap_chain_desc = {};
  swap_chain_desc.nextInChain               = nullptr;
  swap_chain_desc.width                     = w;
  swap_chain_desc.height                    = h;
  swap_chain_desc.format                    = wgpu::TextureFormat::BGRA8Unorm;
  swap_chain_desc.usage       = wgpu::TextureUsage::RenderAttachment;
  swap_chain_desc.presentMode = wgpu::PresentMode::Fifo;
  wgpu::SwapChain swap_chain
      = device.CreateSwapChain(surface, &swap_chain_desc);
  wgpu::RenderPipeline pipeline = jkwgpu::createRenderPipeline(globals);

  while (!glfwWindowShouldClose(window)) {

    // Check whether the user clicked on the close button (and any other
    // mouse/key event, which we don't use so far)
    glfwPollEvents();

    /* get current texture to write to */
    wgpu::TextureView next_tex = swap_chain.GetCurrentTextureView();

    if (!next_tex) {
      std::cerr << "Cannot acquire next swap chain texture" << std::endl;
      break;
    }

    /* create the command encoder */
    wgpu::CommandEncoderDescriptor encoder_desc = {};
    encoder_desc.nextInChain                    = nullptr;
    encoder_desc.label                          = "Default Command Encoder";
    wgpu::CommandEncoder encoder = device.CreateCommandEncoder(&encoder_desc);

    /* describe render pass color attachment */
    wgpu::RenderPassColorAttachment color_attachment = {};
    color_attachment.nextInChain                     = nullptr;
    color_attachment.view                            = next_tex;
    color_attachment.loadOp                          = wgpu::LoadOp::Clear;
    color_attachment.storeOp                         = wgpu::StoreOp::Store;
    color_attachment.clearValue                      = {1, 0, 0, 1};

    /* describe render pass */
    wgpu::RenderPassDescriptor pass_desc = {};
    pass_desc.label                      = "Default Render Pass Encoder";
    pass_desc.colorAttachmentCount       = 1;
    pass_desc.colorAttachments           = &color_attachment;
    pass_desc.timestampWriteCount        = 0;
    pass_desc.timestampWrites            = nullptr;
    pass_desc.depthStencilAttachment     = nullptr;

    /* create render pass encoder */
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&pass_desc);
    pass.SetPipeline(pipeline);

    /* push events to render pass encoder */
    pass.Draw(3, 1, 0, 0);

    /* finish encoding render pass */
    pass.End();

    /* describe the command buffer */
    wgpu::CommandBufferDescriptor command_buffer_desc = {};
    command_buffer_desc.nextInChain                   = nullptr;
    command_buffer_desc.label = "Default Command buffer";

    /* encode the command buffer, push to queue */
    wgpu::CommandBuffer command_buffer = encoder.Finish(&command_buffer_desc);
    queue.Submit(1, &command_buffer);

    /* finally, present the next texture */
    swap_chain.Present();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
