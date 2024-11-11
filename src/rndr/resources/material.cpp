/**
 * @file material.cpp
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "material.h"

namespace rndr {

Material::Material(Context &context) : GlobalAccess(context)
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
      = getDevice().CreateShaderModule(&shader_module_desc);

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

  pipeline_            = getDevice().CreateRenderPipeline(&pipeline_desc);
}

} // namespace rndr
