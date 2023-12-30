/**
 * @file material.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_MATERIAL_H_
#define RNDR_MATERIAL_H_

#include "rndr/types/globals.h"

namespace rndr {

class Material : public GlobalAccess {

  Material(Globals &globals);

  Material(const Material &)            = delete;
  Material &operator=(const Material &) = delete;

  Material(Material &&)                 = delete;
  Material &operator=(Material &&)      = delete;

  /**
   * @brief Set the WGSL shader sources for this material.
   *
   * @note Default entry points to vertex and fragment stages are
   * `rndr_vert_main` and `rndr_frag_main`, respectively.
   *
   * @todo Implement custom entry points.
   */
  void setShaderSource(std::vector<std::string> shader_sources);

  /**
   * @brief Creates this material's render pipeline, rendering it ready
   * for usage in draw calls.
   *
   */
  void         finalize();

  bool         isFinalized();

  virtual void draw() = 0;

protected:
  wgpu::RenderPipeline           pipeline_           = nullptr;
  wgpu::ShaderModule             shader_module_      = nullptr;

  wgpu::RenderPipelineDescriptor desc_               = {};
  wgpu::VertexState              vertex_state_       = {};
  wgpu::BlendState               blend_state_        = {};
  wgpu::ColorTargetState         color_target_state_ = {};
  wgpu::FragmentState            fragment_state_     = {};

  bool                           finalized_          = false;

  std::string                    label_              = "rndr Material";
};

} // namespace rndr

#endif