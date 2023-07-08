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

WGPUAdapter requestWGPUAdapter(WGPUInstance instance)
{
  std::cout << "Requesting adapter..." << std::endl;

  /* elucidate adapter options */
  WGPURequestAdapterOptions req_adapter_opts = {};
  req_adapter_opts.powerPreference = WGPUPowerPreference_HighPerformance;

  /* define user data */
  struct UserData {
    WGPUAdapter adapter = nullptr;
  } udata;

  /* define callback */
  auto cbk = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
                const char *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (adapter == nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->adapter = adapter;
  };

  /* get adapter */
  wgpuInstanceRequestAdapter(instance, &req_adapter_opts, cbk, (void *)&udata);

  /* return */
  std::cout << "Got adapter: " << udata.adapter << std::endl;

  return udata.adapter;
}

WGPUDevice requestWGPUDevice(WGPUAdapter adapter)
{
  std::cout << "Requesting device..." << std::endl;

  size_t feature_count = wgpuAdapterEnumerateFeatures(adapter, nullptr);
  std::vector<WGPUFeatureName> features(feature_count);
  wgpuAdapterEnumerateFeatures(adapter, features.data());

  WGPUQueueDescriptor queue_descriptor;
  queue_descriptor.label           = nullptr;
  queue_descriptor.nextInChain     = nullptr;

  WGPUDeviceDescriptor descriptor  = {};
  descriptor.requiredFeatures      = features.data();
  descriptor.requiredFeaturesCount = features.size();

  WGPUDeviceDescriptor deviceDesc  = {};
  deviceDesc.label                 = "WGPU Default Device";
  deviceDesc.requiredFeaturesCount = 0;
  deviceDesc.requiredLimits        = nullptr;
  deviceDesc.defaultQueue.label    = "WGPU Default Queue";

  struct UserData {
    WGPUDevice device = nullptr;
  } udata;

  auto cbk = [](WGPURequestDeviceStatus status, WGPUDevice device,
                char const *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (device == nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->device = device;
  };

  wgpuAdapterRequestDevice(adapter, &deviceDesc, cbk, (void *)&udata);

  std::cout << "Got device: " << udata.device << std::endl;

  return udata.device;
}

Globals requestGlobals()
{

  Globals globals;

  /* get instance */
  WGPUInstanceDescriptor instance_desc = {};
  instance_desc.nextInChain            = nullptr;

  globals.instance                     = wgpuCreateInstance(&instance_desc);
  globals.adapter                      = requestWGPUAdapter(globals.instance);
  globals.device                       = requestWGPUDevice(globals.adapter);

  wgpuDeviceSetUncapturedErrorCallback(globals.device, wgpu_error_callback,
                                       nullptr);
  wgpuDeviceSetDeviceLostCallback(globals.device, wgpu_device_lost_callback,
                                  nullptr);

  return globals;
}

void releaseGlobals(Globals globals)
{
  wgpuDeviceRelease(globals.device);
  wgpuAdapterRelease(globals.adapter);
  wgpuInstanceRelease(globals.instance);
}

WGPURenderPipeline createRenderPipeline(Globals globals)
{

  const char *shaderSource = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> {
	var p = vec2<f32>(0.0, 0.0);
	if (in_vertex_index == 0u) {
		p = vec2<f32>(-0.5, -0.5);
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
  WGPUShaderModuleWGSLDescriptor shader_code_desc = {};
  // Set the chained struct's header
  shader_code_desc.code        = shaderSource;
  shader_code_desc.chain.next  = nullptr;
  shader_code_desc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;

  WGPUShaderModuleDescriptor shader_module_desc = {};
  shader_module_desc.label                      = "Default Shader Module";
  shader_module_desc.nextInChain                = &shader_code_desc.chain;
  WGPUShaderModule shader_module
      = wgpuDeviceCreateShaderModule(globals.device, &shader_module_desc);

  WGPURenderPipelineDescriptor pipeline_desc = {};
  pipeline_desc.nextInChain                  = nullptr;

  /* pipeline describe vertex stage */
  WGPUVertexState vertex_state = {};
  vertex_state.module          = shader_module;
  vertex_state.entryPoint      = "vs_main";
  vertex_state.bufferCount     = 0;
  vertex_state.buffers         = nullptr;
  vertex_state.constantCount   = 0;
  vertex_state.constants       = nullptr;
  pipeline_desc.vertex         = std::move(vertex_state);

  /* pipeline describe primitive stage */
  pipeline_desc.primitive.topology         = WGPUPrimitiveTopology_TriangleList;
  pipeline_desc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
  pipeline_desc.primitive.frontFace        = WGPUFrontFace_CCW;
  pipeline_desc.primitive.cullMode         = WGPUCullMode_None;

  /* pipeline describe fragment stage */
  WGPUBlendState blend_state              = {};
  blend_state.color.srcFactor             = WGPUBlendFactor_SrcAlpha;
  blend_state.color.dstFactor             = WGPUBlendFactor_OneMinusSrcAlpha;
  blend_state.color.operation             = WGPUBlendOperation_Add;
  blend_state.alpha.srcFactor             = WGPUBlendFactor_One;
  blend_state.alpha.dstFactor             = WGPUBlendFactor_Zero;
  blend_state.alpha.operation             = WGPUBlendOperation_Add;

  WGPUColorTargetState color_target_state = {};
  color_target_state.blend                = &blend_state;
  color_target_state.format               = WGPUTextureFormat_BGRA8Unorm;
  color_target_state.writeMask            = WGPUColorWriteMask_All;

  WGPUFragmentState fragment_state        = {};
  fragment_state.module                   = shader_module;
  fragment_state.entryPoint               = "fs_main";
  fragment_state.targetCount              = 1;
  fragment_state.targets                  = &color_target_state;

  pipeline_desc.fragment                  = &fragment_state;

  /* pipeline depth/stencil settings */
  pipeline_desc.depthStencil = nullptr;

  /* pipeline describe multisampling */
  pipeline_desc.multisample.count                  = 1;
  pipeline_desc.multisample.mask                   = ~0U;
  pipeline_desc.multisample.alphaToCoverageEnabled = false;

  /* data layout */
  pipeline_desc.layout = nullptr;

  WGPURenderPipeline ret
      = wgpuDeviceCreateRenderPipeline(globals.device, &pipeline_desc);

  wgpuShaderModuleRelease(shader_module);

  return ret;
}

} // namespace jkwgpu