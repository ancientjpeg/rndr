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

#include <iostream>
#include <vector>

namespace rndr {

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
  descriptor.requiredFeatureCount   = 0;
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

_Globals requestGlobals(GLFWwindow *window)
{

  _Globals globals;

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

wgpu::RenderPipeline createRenderPipeline(_Globals globals)
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

namespace helpers {
void default_device_lost_callback(WGPUDeviceLostReason reason,
                                  char const          *message,
                                  void                *userdata)
{
  if (reason != WGPUDeviceLostReason_Destroyed) {
    throw std::runtime_error(message);
  }
  std::cout << "Device successfully destroyed.";
}

void default_error_callback(WGPUErrorType type,
                            char const   *message,
                            void         *userdata)
{
  throw std::runtime_error(message);
}

template <typename T>
bool limits_test(T sup, T req, T def)
{
  return sup >= req || req == def;
}

bool limits_supported(wgpu::Limits required_limits,
                      wgpu::Limits supported_limits)
{
  wgpu::Limits def  = {};
  bool         temp = true;
  temp              = temp
         && limits_test(supported_limits.maxTextureDimension1D,
                        required_limits.maxTextureDimension1D,
                        def.maxTextureDimension1D);
  temp = temp
         && limits_test(supported_limits.maxTextureDimension2D,
                        required_limits.maxTextureDimension2D,
                        def.maxTextureDimension2D);
  temp = temp
         && limits_test(supported_limits.maxTextureDimension3D,
                        required_limits.maxTextureDimension3D,
                        def.maxTextureDimension3D);
  temp = temp
         && limits_test(supported_limits.maxTextureArrayLayers,
                        required_limits.maxTextureArrayLayers,
                        def.maxTextureArrayLayers);
  temp = temp
         && limits_test(supported_limits.maxBindGroups,
                        required_limits.maxBindGroups, def.maxBindGroups);
  temp = temp
         && limits_test(supported_limits.maxBindGroupsPlusVertexBuffers,
                        required_limits.maxBindGroupsPlusVertexBuffers,
                        def.maxBindGroupsPlusVertexBuffers);
  temp = temp
         && limits_test(supported_limits.maxBindingsPerBindGroup,
                        required_limits.maxBindingsPerBindGroup,
                        def.maxBindingsPerBindGroup);
  temp = temp
         && limits_test(
             supported_limits.maxDynamicUniformBuffersPerPipelineLayout,
             required_limits.maxDynamicUniformBuffersPerPipelineLayout,
             def.maxDynamicUniformBuffersPerPipelineLayout);
  temp = temp
         && limits_test(
             supported_limits.maxDynamicStorageBuffersPerPipelineLayout,
             required_limits.maxDynamicStorageBuffersPerPipelineLayout,
             def.maxDynamicStorageBuffersPerPipelineLayout);
  temp = temp
         && limits_test(supported_limits.maxSampledTexturesPerShaderStage,
                        required_limits.maxSampledTexturesPerShaderStage,
                        def.maxSampledTexturesPerShaderStage);
  temp = temp
         && limits_test(supported_limits.maxSamplersPerShaderStage,
                        required_limits.maxSamplersPerShaderStage,
                        def.maxSamplersPerShaderStage);
  temp = temp
         && limits_test(supported_limits.maxStorageBuffersPerShaderStage,
                        required_limits.maxStorageBuffersPerShaderStage,
                        def.maxStorageBuffersPerShaderStage);
  temp = temp
         && limits_test(supported_limits.maxStorageTexturesPerShaderStage,
                        required_limits.maxStorageTexturesPerShaderStage,
                        def.maxStorageTexturesPerShaderStage);
  temp = temp
         && limits_test(supported_limits.maxUniformBuffersPerShaderStage,
                        required_limits.maxUniformBuffersPerShaderStage,
                        def.maxUniformBuffersPerShaderStage);
  temp = temp
         && limits_test(supported_limits.maxUniformBufferBindingSize,
                        required_limits.maxUniformBufferBindingSize,
                        def.maxUniformBufferBindingSize);
  temp = temp
         && limits_test(supported_limits.maxStorageBufferBindingSize,
                        required_limits.maxStorageBufferBindingSize,
                        def.maxStorageBufferBindingSize);
  temp = temp
         && limits_test(supported_limits.minUniformBufferOffsetAlignment,
                        required_limits.minUniformBufferOffsetAlignment,
                        def.minUniformBufferOffsetAlignment);
  temp = temp
         && limits_test(supported_limits.minStorageBufferOffsetAlignment,
                        required_limits.minStorageBufferOffsetAlignment,
                        def.minStorageBufferOffsetAlignment);
  temp = temp
         && limits_test(supported_limits.maxVertexBuffers,
                        required_limits.maxVertexBuffers, def.maxVertexBuffers);
  temp = temp
         && limits_test(supported_limits.maxBufferSize,
                        required_limits.maxBufferSize, def.maxBufferSize);
  temp = temp
         && limits_test(supported_limits.maxVertexAttributes,
                        required_limits.maxVertexAttributes,
                        def.maxVertexAttributes);
  temp = temp
         && limits_test(supported_limits.maxVertexBufferArrayStride,
                        required_limits.maxVertexBufferArrayStride,
                        def.maxVertexBufferArrayStride);
  temp = temp
         && limits_test(supported_limits.maxInterStageShaderComponents,
                        required_limits.maxInterStageShaderComponents,
                        def.maxInterStageShaderComponents);
  temp = temp
         && limits_test(supported_limits.maxInterStageShaderVariables,
                        required_limits.maxInterStageShaderVariables,
                        def.maxInterStageShaderVariables);
  temp = temp
         && limits_test(supported_limits.maxColorAttachments,
                        required_limits.maxColorAttachments,
                        def.maxColorAttachments);
  temp = temp
         && limits_test(supported_limits.maxColorAttachmentBytesPerSample,
                        required_limits.maxColorAttachmentBytesPerSample,
                        def.maxColorAttachmentBytesPerSample);
  temp = temp
         && limits_test(supported_limits.maxComputeWorkgroupStorageSize,
                        required_limits.maxComputeWorkgroupStorageSize,
                        def.maxComputeWorkgroupStorageSize);
  temp = temp
         && limits_test(supported_limits.maxComputeInvocationsPerWorkgroup,
                        required_limits.maxComputeInvocationsPerWorkgroup,
                        def.maxComputeInvocationsPerWorkgroup);
  temp = temp
         && limits_test(supported_limits.maxComputeWorkgroupSizeX,
                        required_limits.maxComputeWorkgroupSizeX,
                        def.maxComputeWorkgroupSizeX);
  temp = temp
         && limits_test(supported_limits.maxComputeWorkgroupSizeY,
                        required_limits.maxComputeWorkgroupSizeY,
                        def.maxComputeWorkgroupSizeY);
  temp = temp
         && limits_test(supported_limits.maxComputeWorkgroupSizeZ,
                        required_limits.maxComputeWorkgroupSizeZ,
                        def.maxComputeWorkgroupSizeZ);
  temp = temp
         && limits_test(supported_limits.maxComputeWorkgroupsPerDimension,
                        required_limits.maxComputeWorkgroupsPerDimension,
                        def.maxComputeWorkgroupsPerDimension);
  return temp;
}

} // namespace helpers

} // namespace rndr
