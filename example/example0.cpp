#include "rndr/application.h"
#include "rndr/math/ops.h"
#include "rndr/utils/helpers.h"
#include <cassert>
#include <iostream>
#include <sstream>

constexpr int w = 640;
constexpr int h = 480;

rndr::result  performBufferCopies(rndr::Application &program_gpu)
{
  const wgpu::Device &device = program_gpu.getDevice();

  /* buffer write test */
  wgpu::BufferDescriptor buffer_desc;
  buffer_desc.label = "Some GPU-side data buffer";
  buffer_desc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc;
  buffer_desc.size  = 16 * sizeof(float);
  buffer_desc.mappedAtCreation = false;
  wgpu::Buffer buffer1         = device.CreateBuffer(&buffer_desc);

  buffer_desc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
  wgpu::Buffer       buffer2 = device.CreateBuffer(&buffer_desc);

  std::vector<float> nums(16), nums_out(16);
  std::fill(nums.begin(), nums.end(), 5.2f);
  std::fill(nums_out.begin(), nums_out.end(), 0.2f);

  program_gpu.getQueue().WriteBuffer(buffer1, 0, nums.data(),
                                     nums.size() * sizeof(float));

  wgpu::CommandEncoderDescriptor enc_desc{};
  wgpu::CommandEncoder           enc = device.CreateCommandEncoder(&enc_desc);
  enc.CopyBufferToBuffer(buffer1, 0, buffer2, 0, 16 * sizeof(float));
  wgpu::CommandBufferDescriptor buf_desc{};
  wgpu::CommandBuffer           temp_cbuf = enc.Finish(&buf_desc);
  program_gpu.getQueue().Submit(1, &temp_cbuf);
  auto copy_future = program_gpu.getSubmittedWorkFuture();
  program_gpu.processEvents();
  program_gpu.blockOnFuture(copy_future);

  /* buffer 2 map operation */
  struct Context {
    std::vector<float> &out;
    wgpu::Buffer        buffer;
  };
  Context ctx{nums_out, buffer2};

  auto    cbk = [&](wgpu::MapAsyncStatus status, const char *message) {
    float *range = (float *)buffer2.GetConstMappedRange(0, 16 * sizeof(float));
    std::copy(range, range + 16, nums_out.begin());
    bool equal = std::equal(range, range + nums.size(), nums.begin());
    std::cout << "BUFFER2 COPY SUCCESS?:" << equal << std::endl;
    buffer2.Unmap();
  };

  buffer2.MapAsync(wgpu::MapMode::Read, 0, 16 * sizeof(float),
                   wgpu::CallbackMode::AllowProcessEvents, cbk);

  program_gpu.processEvents();
  auto map_future = program_gpu.getSubmittedWorkFuture();
  program_gpu.blockOnFuture(map_future);

  buffer1.Destroy();
  buffer2.Destroy();

  return {};
}

rndr::result renderFrame(rndr::Application &program_gpu)
{

  const wgpu::Device  &device   = program_gpu.getDevice();

  wgpu::RenderPipeline pipeline = rndr::createRenderPipeline(program_gpu);

  /* check async ops */
  program_gpu.processEvents();

  /* get current texture to write to */
  wgpu::SurfaceTexture surface_tex;
  program_gpu.getSurface().GetCurrentTexture(&surface_tex);

  if (surface_tex.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
    std::ostringstream oss;
    oss << "Surface did not provide next texture. "
           "SurfaceGetCurrentTextureStatus:"
        << static_cast<uint32_t>(surface_tex.status);
    return rndr::unexpected(oss.str());
  }

  wgpu::Texture               next_tex_src = surface_tex.texture;
  wgpu::TextureViewDescriptor next_tex_descriptor;

  next_tex_descriptor.format = next_tex_src.GetFormat();
  assert(next_tex_src.GetDimension() == wgpu::TextureDimension::e2D);
  next_tex_descriptor.dimension = wgpu::TextureViewDimension::e2D;

  wgpu::TextureView next_tex    = next_tex_src.CreateView(&next_tex_descriptor);

  if (!next_tex) {
    return rndr::unexpected("Cannot acquire next swap chain texture");
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
  color_attachment.clearValue                      = {0, 0, 0, 1};

  /* describe render pass */
  wgpu::RenderPassDescriptor pass_desc = {};
  pass_desc.label                      = "Default Render Pass Encoder";
  pass_desc.colorAttachmentCount       = 1;
  pass_desc.colorAttachments           = &color_attachment;
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
  command_buffer_desc.label                         = "Default Command buffer";

  /* encode the command buffer, push to queue */
  wgpu::CommandBuffer command_buffer = encoder.Finish(&command_buffer_desc);
  program_gpu.getQueue().Submit(1, &command_buffer);

  wgpu::Future work_future = program_gpu.getSubmittedWorkFuture();
  /* program_gpu.blockOnFuture(work_future); */

  /* finally, present the next texture */
  program_gpu.getSurface().Present();

  return {};
}

int main()
{
  rndr::Application program_gpu;
  rndr::result      init_result = program_gpu.initialize();

  if (!init_result.ok()) {
    std::cerr << "Initialization failed for reason: " << init_result;
    return 1;
  }

  rndr::result buffer_copy_result = performBufferCopies(program_gpu);
  if (!buffer_copy_result) {
    std::cerr << buffer_copy_result << std::endl;
    return 1;
  }

  rndr::result render_result
      = rndr::unexpected("Did not complete first frame.");

  do {
    if (glfwWindowShouldClose(program_gpu.getWindow())) {
      render_result = {};
      break;
    }
    // Check whether the user clicked on the close button (and any other
    // mouse/key event, which we don't use so far)
    glfwPollEvents();

    render_result = renderFrame(program_gpu);
  } while (render_result);

  auto &stream = render_result ? std::cout : std::cerr;
  std::cout << render_result << std::endl;

  return render_result ? 0 : 1;
}
