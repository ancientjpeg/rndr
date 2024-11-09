/**
 * @file pipeline_base.h
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright (c) 2024. All rights reserved.
 */

#ifndef RNDR_PIPELINE_BASE_H_
#define RNDR_PIPELINE_BASE_H_

#include "ustd/expected.h"
#include <webgpu/webgpu_cpp.h>

namespace rndr {

enum class PipelinePriority {
  Static        = 0,
  PerFrame      = 1,
  PerObject     = 2,
  NumBindGroups = 3
};

class PipelineBase {
public:
  /**
   * @brief Commits this pipeline layout and creates its resources on the GPU
   */
  ustd::result commit(wgpu::Device &device);

protected:
  struct BindingIdentifier {
    uint32_t group;
    uint32_t binding;
  };

  struct BindingMeta {
    PipelinePriority  priority;
    uint64_t          size;
    wgpu::BufferUsage usage
        = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
  };

  /**
   * @brief Create and add a buffer to this pipeline
   *
   * @param priority Determines the bind group that this buffer will bind to.
   * @param size The size of the buffer to be allocated.
   * @return A `std::pair` containing the bind group and binding indices for the
   * newly created binding.
   */
  ustd::expected<BindingIdentifier> createBuffer(BindingMeta meta);

  ustd::expected<wgpu::Buffer>      getBuffer(uint32_t group, uint32_t binding);

  static PipelinePriority           getPriorityForGroup(uint32_t group)
  {
    return static_cast<PipelinePriority>(group);
  }

private:
  struct BindingData {
    BindingIdentifier identifier;
    BindingMeta       meta;
    wgpu::Buffer      buffer;
  };

  struct BindGroupData {
    std::vector<BindingData> bindings;
    wgpu::BindGroupLayout    bind_group_layout;
    wgpu::BindGroup          bind_group;
  };

  static constexpr size_t bind_group_count_
      = static_cast<size_t>(PipelinePriority::NumBindGroups);

  std::array<BindGroupData, bind_group_count_> bind_groups_metadata;
};

} // namespace rndr

#endif
