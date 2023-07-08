/**
 * @file init.cpp
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "helpers.h"
#include "glfw3webgpu.h"

#include <exception>
#include <iostream>
#include <vector>

namespace jkwgpu {

void wgpu_error_callback(WGPUErrorType type, char const *message, void *)
{
  std::cerr << "WGPU ERROR TYPE: " << type << std::endl;
  std::cerr << "WGPU ERROR MESSAGE: " << message << std::endl << std::endl;
}

void wgpu_device_lost_callback(WGPUDeviceLostReason reason,
                               char const          *message,
                               void *)
{
  std::cerr << "WGPU DEVICE LOST REASON: " << reason << std::endl;
  std::cerr << "WGPU DEVICE LOST MESSAGE: " << message << std::endl
            << std::endl;
}

wgpu::Adapter requestWGPUAdapter(wgpu::Instance &instance,
                                 wgpu::Surface  &surface)
{
  std::cout << "Requesting adapter..." << std::endl;

  /* elucidate adapter options */
  wgpu::RequestAdapterOptions req_adapter_opts = {};
  req_adapter_opts.powerPreference = wgpu::PowerPreference::HighPerformance;
  req_adapter_opts.forceFallbackAdapter = false;
  req_adapter_opts.compatibleSurface    = surface;

  /* define user data */
  struct UserData {
    wgpu::Adapter adapter;
  } udata;

  /* define callback */
  auto cbk = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
                const char *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (adapter == nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->adapter = wgpu::Adapter::Acquire(adapter);
  };

  /* get adapter */
  instance.RequestAdapter(&req_adapter_opts, cbk, (void *)&udata);

  /* return */
  std::cout << "Got adapter: " << udata.adapter.Get() << std::endl;

  return std::move(udata.adapter);
}

wgpu::Device requestWGPUDevice(wgpu::Adapter adapter)
{
  std::cout << "Requesting device..." << std::endl;

  /* check adapter features */
  size_t feature_count = adapter.EnumerateFeatures(nullptr);
  std::vector<wgpu::FeatureName> features(feature_count);
  adapter.EnumerateFeatures(features.data());

  /* describe the device */
  wgpu::DeviceDescriptor descriptor = {};
  descriptor.requiredFeatures       = nullptr;
  descriptor.requiredFeaturesCount  = 0;
  descriptor.requiredFeaturesCount  = 0;
  descriptor.requiredLimits         = nullptr;
  descriptor.label                  = "WGPU Default Device";
  descriptor.defaultQueue.label     = "WGPU Default Queue";

  struct UserData {
    wgpu::Device device = nullptr;
  } udata;

  auto cbk = [](WGPURequestDeviceStatus status, WGPUDevice device,
                char const *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (device == nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->device = wgpu::Device::Acquire(device);
  };

  adapter.RequestDevice(&descriptor, cbk, (void *)&udata);

  std::cout << "Got device: " << udata.device.Get() << std::endl;

  return udata.device;
}

Globals requestGlobals(GLFWwindow *window)
{

  Globals globals;

  /* get instance */
  wgpu::InstanceDescriptor instance_desc = {};
  instance_desc.nextInChain              = nullptr;

  globals.instance                       = wgpu::CreateInstance(&instance_desc);
  globals.surface                        = wgpu::Surface::Acquire(
      glfwGetWGPUSurface(globals.instance.Get(), window));

  globals.adapter = requestWGPUAdapter(globals.instance, globals.surface);
  globals.device  = requestWGPUDevice(globals.adapter);

  globals.device.SetUncapturedErrorCallback(wgpu_error_callback, nullptr);
  globals.device.SetDeviceLostCallback(wgpu_device_lost_callback, nullptr);

  return globals;
}

wgpu::RenderPipeline createRenderPipeline(Globals globals)
{

  const char *shaderSource = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> 
{ 	
  var p = vec2<f32>(0.0, 0.0); 	
  if (in_vertex_index == 0u) {
    p = vec2<f32>(-0.5, -0.8); 	
  } else if (in_vertex_index == 1u) { 
    p = vec2<f32>(0.5, -0.5); 	
  } else { 		
    p = vec2<f32>(0.0, 0.5);
  }
	return vec4<f32>(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4<f32> {
    return vec4<f32>(0.0, 0.4, 1.0, 1.0);
}

)";

  /* create shader module */
  wgpu::ShaderModuleWGSLDescriptor shader_code_desc = {};
  // Set the chained struct's header
  shader_code_desc.code                           = shaderSource;

  wgpu::ShaderModuleDescriptor shader_module_desc = {};
  shader_module_desc.label                        = "Default Shader Module";
  shader_module_desc.nextInChain                  = &shader_code_desc;
  wgpu::ShaderModule shader_module
      = globals.device.CreateShaderModule(&shader_module_desc);

  wgpu::RenderPipelineDescriptor pipeline_desc = {};
  pipeline_desc.nextInChain                    = nullptr;

  /* pipeline describe vertex stage */
  wgpu::VertexState vertex_state = {};
  vertex_state.module            = shader_module;
  vertex_state.entryPoint        = "vs_main";
  vertex_state.bufferCount       = 0;
  vertex_state.buffers           = nullptr;
  vertex_state.constantCount     = 0;
  vertex_state.constants         = nullptr;
  pipeline_desc.vertex           = std::move(vertex_state);

  /* pipeline describe primitive stage */
  pipeline_desc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
  pipeline_desc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
  pipeline_desc.primitive.frontFace        = wgpu::FrontFace::CCW;
  pipeline_desc.primitive.cullMode         = wgpu::CullMode::None;

  /* pipeline describe fragment stage */
  wgpu::BlendState blend_state = {};
  blend_state.color.srcFactor  = wgpu::BlendFactor::SrcAlpha;
  blend_state.color.dstFactor  = wgpu::BlendFactor::OneMinusSrcAlpha;
  blend_state.color.operation  = wgpu::BlendOperation::Add;
  blend_state.alpha.srcFactor  = wgpu::BlendFactor::One;
  blend_state.alpha.dstFactor  = wgpu::BlendFactor::Zero;
  blend_state.alpha.operation  = wgpu::BlendOperation::Add;

  wgpu::ColorTargetState color_target_state = {};
  color_target_state.blend                  = &blend_state;
  color_target_state.format                 = wgpu::TextureFormat::BGRA8Unorm;
  color_target_state.writeMask              = wgpu::ColorWriteMask::All;

  wgpu::FragmentState fragment_state        = {};
  fragment_state.module                     = shader_module;
  fragment_state.entryPoint                 = "fs_main";
  fragment_state.targetCount                = 1;
  fragment_state.targets                    = &color_target_state;

  pipeline_desc.fragment                    = &fragment_state;

  /* pipeline depth/stencil settings */
  pipeline_desc.depthStencil = nullptr;

  /* pipeline describe multisampling */
  pipeline_desc.multisample.count                  = 1;
  pipeline_desc.multisample.mask                   = ~0U;
  pipeline_desc.multisample.alphaToCoverageEnabled = false;

  /* data layout */
  pipeline_desc.layout = nullptr;

  wgpu::RenderPipeline ret
      = globals.device.CreateRenderPipeline(&pipeline_desc);

  return ret;
}

} // namespace jkwgpu