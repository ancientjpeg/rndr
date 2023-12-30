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
  Material    &operator=(Material &&)   = delete;

  void         setShaderSource();

  virtual void draw() = 0;

protected:
  wgpu::RenderPipelineDescriptor desc_;
  wgpu::RenderPipeline           pipeline_;
};

} // namespace rndr

#endif