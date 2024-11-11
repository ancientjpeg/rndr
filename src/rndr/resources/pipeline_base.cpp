/**
 * @file pipeline_base.cpp
 * @author Jackson Kaplan (jackson@minimal.audio)
 * @date 2024-11-07
 * Copyright (c) 2024. All rights reserved.
 */

#include "pipeline_base.h"

namespace rndr {

ustd::result PipelineBase::commit(wgpu::Device &device)
{
  for (BindGroupData &group_meta : bind_groups_metadata) {

    std::vector<wgpu::BindGroupLayoutEntry> bg_layout_entries;
    std::vector<wgpu::BindGroupEntry>       bg_entries;
    for (BindingData &binding_data : group_meta.bindings) {

      wgpu::BufferDescriptor bfd;
      bfd.size             = binding_data.meta.size;
      bfd.usage            = binding_data.meta.usage;
      bfd.mappedAtCreation = false;
      wgpu::Buffer buf     = device.CreateBuffer(&bfd);
      binding_data.buffer  = std::move(buf);

      /* BindGroup */
      wgpu::BufferBindingLayout buffer_binding_layout;

      const auto               &usage = binding_data.meta.usage;

      if (usage & wgpu::BufferUsage::Uniform) {
        buffer_binding_layout.type = wgpu::BufferBindingType::Uniform;
      }
      else if (usage & wgpu::BufferUsage::Storage) {
        buffer_binding_layout.type = wgpu::BufferBindingType::Storage;
      }
      else {
        return ustd::unexpected("Got unexpected buffer usage incompatible with "
                                "wgpu::BufferBindingType");
      }

      wgpu::BindGroupLayoutEntry bg_layout_entry;
      bg_layout_entry.binding = binding_data.identifier.binding;
      bg_layout_entry.buffer  = buffer_binding_layout;

      bg_layout_entries.push_back(bg_layout_entry);

      wgpu::BindGroupEntry bg_entry;
      bg_entry.binding = binding_data.identifier.binding;
      bg_entry.buffer  = binding_data.buffer;
      bg_entry.offset  = 0;
      bg_entry.size    = binding_data.buffer.GetSize();
    }

    wgpu::BindGroupLayoutDescriptor bg_layout_desc;
    bg_layout_desc.entries    = bg_layout_entries.data();
    bg_layout_desc.entryCount = bg_layout_entries.size();

    group_meta.bind_group_layout
        = device.CreateBindGroupLayout(&bg_layout_desc);

    wgpu::BindGroupDescriptor bg_desc;
    bg_desc.layout        = group_meta.bind_group_layout;
    bg_desc.entries       = bg_entries.data();
    bg_desc.entryCount    = bg_entries.size();

    group_meta.bind_group = device.CreateBindGroup(&bg_desc);
  }

  return {};
}

ustd::expected<PipelineBase::BindingIdentifier>
PipelineBase::createBuffer(BindingMeta meta)
{
  size_t group_idx = static_cast<size_t>(meta.priority);
  if (group_idx > bind_groups_metadata.size()) {
    return ustd::unexpected("Group index out of range: "
                            + std::to_string(group_idx));
  }

  auto       &bg_meta = bind_groups_metadata[group_idx];
  uint32_t    binding = static_cast<uint32_t>(bg_meta.bindings.size());

  BindingData binding_data;
  binding_data.identifier.binding = binding;
  binding_data.identifier.group   = group_idx;

  bg_meta.bindings.push_back(binding_data);
  return binding_data.identifier;
}

} // namespace rndr
