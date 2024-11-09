/**
 * @file pipeline_base.h
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright © 2024 Minimal. All rights reserved.
 */

#ifndef RNDR_PIPELINE_BASE_H_
#define RNDR_PIPELINE_BASE_H_

#include "ustd/expected.h"
#include <map>
#include <webgpu/webgpu_cpp.h>

namespace rndr {

enum class PipelinePriority {
  Static        = 0,
  PerFrame      = 1,
  PerObject     = 2,
  NumBindGroups = 3
};

class PipelineBase {
protected:
  struct BindingMeta {
    uint32_t     group;
    uint32_t     binding;
    wgpu::Buffer buffer;
    uint64_t     size()
    {
      return buffer.GetSize();
    };
  };
  /**
   * @brief Create and add a buffer to this pipeline
   *
   * @param priority Determines the bind group that this buffer will bind to.
   * @param size The size of the buffer to be allocated.
   * @return A `std::pair` containing the bind group and binding indices for the
   * newly created binding.
   */
  ustd::expected<BindingMeta> createBuffer(wgpu::Device     &device,
                                           PipelinePriority  priority,
                                           uint64_t          size,
                                           wgpu::BufferUsage usage
                                           = wgpu::BufferUsage::CopyDst
                                             | wgpu::BufferUsage::Uniform);

  ustd::expected<BindingMeta> getBuffer(uint32_t group, uint32_t binding);

  static PipelinePriority     getPriorityForGroup(uint32_t group)
  {
    return static_cast<PipelinePriority>(group);
  }

private:
  static constexpr size_t bind_group_count_
      = static_cast<size_t>(PipelinePriority::NumBindGroups);

  struct BindGroupMeta {
    PipelinePriority         priority;
    std::vector<BindingMeta> bindings;
  };

  std::array<BindGroupMeta, bind_group_count_> buffer_bindings_;
};

} // namespace rndr

#endif
