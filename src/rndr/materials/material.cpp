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
#include "rndr/utils/helpers.h"

namespace rndr {

Material::Material(Globals &globals) : GlobalAccess(globals)
{
}

void Material::finalize()
{
  if (shader_module_.Get() == nullptr) {
    throw std::runtime_error(
        "Shader source must be set before material finalization.");
  }

  wgpu::RenderPipelineDescriptor pipeline_desc_ = {};
  pipeline_desc_.nextInChain                    = nullptr;

  /* pipeline describe vertex stage */
  vertex_state_.module        = shader_module_;
  vertex_state_.entryPoint    = "rndr_vert_main";
  vertex_state_.bufferCount   = 0;
  vertex_state_.buffers       = nullptr;
  vertex_state_.constantCount = 0;
  vertex_state_.constants     = nullptr;
  pipeline_desc_.vertex       = vertex_state_;

  /* pipeline describe primitive stage */
  pipeline_desc_.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
  pipeline_desc_.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
  pipeline_desc_.primitive.frontFace        = wgpu::FrontFace::CCW;
  pipeline_desc_.primitive.cullMode         = wgpu::CullMode::None;

  /* pipeline describe fragment stage */
  blend_state_.color.srcFactor  = wgpu::BlendFactor::SrcAlpha;
  blend_state_.color.dstFactor  = wgpu::BlendFactor::OneMinusSrcAlpha;
  blend_state_.color.operation  = wgpu::BlendOperation::Add;
  blend_state_.alpha.srcFactor  = wgpu::BlendFactor::One;
  blend_state_.alpha.dstFactor  = wgpu::BlendFactor::Zero;
  blend_state_.alpha.operation  = wgpu::BlendOperation::Add;

  color_target_state_.blend     = &blend_state_;
  color_target_state_.format    = wgpu::TextureFormat::BGRA8Unorm;
  color_target_state_.writeMask = wgpu::ColorWriteMask::All;

  fragment_state_.module        = shader_module_;
  fragment_state_.entryPoint    = "rndr_frag_main";
  fragment_state_.targetCount   = 1;
  fragment_state_.targets       = &color_target_state_;

  pipeline_desc_.fragment       = &fragment_state_;

  /* pipeline depth/stencil settings */
  pipeline_desc_.depthStencil = nullptr;

  /* pipeline describe multisampling */
  pipeline_desc_.multisample.count                  = 1;
  pipeline_desc_.multisample.mask                   = ~0U;
  pipeline_desc_.multisample.alphaToCoverageEnabled = false;

  /* data layout */
  wgpu::PipelineLayoutDescriptor desc;
  wgpu::PipelineLayout           layout;
  pipeline_desc_.layout = nullptr;

  pipeline_             = getDevice().CreateRenderPipeline(&pipeline_desc_);

  finalized_            = true;
}

void Material::setShaderSource(std::vector<std::string> shader_sources)
{
  std::string label = label_ + " Shader Module";
  shader_module_
      = helpers::createShaderModule(getDevice(), shader_sources, label.c_str());
}

} // namespace rndr