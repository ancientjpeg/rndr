#include "wgpu_init.h"
#include <cassert>
#include <exception>
#include <iostream>
#include <vector>

WGPUAdapter requestAdapter()
{
  std::cout << "Requesting adapter..." << std::endl;

  /* get instance */
  WGPUInstanceDescriptor instance_desc = {};
  instance_desc.nextInChain            = nullptr;
  WGPUInstance instance                = wgpuCreateInstance(&instance_desc);

  /* elucidate adapter options */
  WGPURequestAdapterOptions req_adapter_opts;
  req_adapter_opts.nextInChain          = nullptr;
  req_adapter_opts.compatibleSurface    = nullptr;
  req_adapter_opts.powerPreference      = WGPUPowerPreference_HighPerformance;
  req_adapter_opts.forceFallbackAdapter = false;

  /* define user data */
  struct UserData {
    WGPUAdapter adapter = nullptr;
  } udata;

  /* define callback */
  auto cbk = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
                const char *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (adapter != nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->adapter = adapter;
  };

  /* get adapter */
  WGPUAdapter adapter = {};
  wgpuInstanceRequestAdapter(instance, &req_adapter_opts, cbk, (void *)&udata);

  /* return */
  std::cout << "Got adapter: " << adapter << std::endl;

  return udata.adapter;
}

WGPUDevice requestWGPUDevice()
{
  WGPUAdapter adapter;
  std::cout << "Requesting device..." << std::endl;
  try {
    adapter = requestAdapter();
  }
  catch (const std::exception &e) {
    throw e;
  }

  size_t feature_count = wgpuAdapterEnumerateFeatures(adapter, nullptr);
  std::vector<WGPUFeatureName> features(feature_count);
  wgpuAdapterEnumerateFeatures(adapter, features.data());

  WGPUQueueDescriptor queue_descriptor;
  queue_descriptor.label           = nullptr;
  queue_descriptor.nextInChain     = nullptr;

  WGPUDevice           device      = {};
  WGPUDeviceDescriptor descriptor  = {};
  descriptor.nextInChain           = nullptr;
  descriptor.label                 = nullptr;
  descriptor.requiredFeatures      = features.data();
  descriptor.requiredFeaturesCount = features.size();
  descriptor.requiredLimits        = nullptr;
  descriptor.defaultQueue          = queue_descriptor;

  struct UserData {
    WGPUDevice device = nullptr;
  } udata;

  auto cbk = [](WGPURequestDeviceStatus status, WGPUDevice device,
                char const *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (device != nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->device = device;
  };

  wgpuAdapterRequestDevice(adapter, &descriptor, cbk, (void *)&udata);

  std::cout << "Got device: " << device << std::endl;

  return device;
}
