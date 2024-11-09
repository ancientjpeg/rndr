/**
 * @file pipeline_base.cpp
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright © 2024 Minimal. All rights reserved.
 */

#include "pipeline_base.h"

namespace rndr {

Res<PipelineBase::BindingMeta>
PipelineBase::createBuffer(wgpu::Device     &device,
                           PipelinePriority  priority,
                           uint64_t          size,
                           wgpu::BufferUsage usage)
{
  size_t group_idx = static_cast<size_t>(priority);
  if (group_idx > buffer_bindings_.size()) {
    return Failure("Group index out of range: " + std::to_string(group_idx));
  }

  wgpu::BufferDescriptor bfd;
  bfd.size             = size;
  bfd.usage            = usage;
  bfd.mappedAtCreation = false;
  wgpu::Buffer buf     = device.CreateBuffer(&bfd);

  auto        &bg_meta = buffer_bindings_[group_idx];
  uint32_t     binding = static_cast<uint32_t>(bg_meta.bindings.size());
  bg_meta.bindings.push_back(/*binding*/)
}

} // namespace rndr