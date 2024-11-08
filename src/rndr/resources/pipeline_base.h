/**
 * @file pipeline_base.h
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright © 2024 Minimal. All rights reserved.
 */

#ifndef RNDR_PIPELINE_BASE_H_
#define RNDR_PIPELINE_BASE_H_

#include "rndr/utils/result.h"
#include <map>
#include <webgpu/webgpu_cpp.h>

namespace rndr {

enum class PipelinePriority { Static, PerFrame, PerObject, NumBindGroups };

class PipelineBase {
protected:
  /**
   * @brief Create and add a buffer to this pipeline
   *
   * @param priority Determines the bind group that this buffer will bind to.
   * @param size The size of the buffer to be allocated.
   * @return A `std::pair` containing the bind group and binding indices for the
   * newly created binding.
   */
  Res<std::pair<uint32_t, uint32_t>> createBuffer(PipelinePriority priority,
                                                  uint64_t         size);

private:
  static constexpr size_t bind_group_count_
      = static_cast<size_t>(PipelinePriority::NumBindGroups);

  struct BufferBinding {
    uint32_t     group;
    uint32_t     binding;
    wgpu::Buffer buffer;
  }

  using BindGroupMap
      = std::map<uint32_t, BufferBinding>;

  std::array<BindGroupMap, bind_group_count_> buffer_bindings_;
};

} // namespace rndr

#endif
