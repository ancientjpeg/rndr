#include "wgpu_init.h"
#include "glfw3webgpu.h"
#include <cassert>
#include <exception>
#include <iostream>
#include <vector>

WGPUAdapter requestWGPUAdapter()
{
  std::cout << "Requesting adapter..." << std::endl;

  /* get instance */
  WGPUInstanceDescriptor instance_desc = {};
  instance_desc.nextInChain            = nullptr;
  WGPUInstance instance                = wgpuCreateInstance(&instance_desc);

  /* elucidate adapter options */
  WGPURequestAdapterOptions req_adapter_opts = {};
  req_adapter_opts.powerPreference = WGPUPowerPreference_HighPerformance;

  /* define user data */
  struct UserData {
    WGPUAdapter adapter = nullptr;
  } udata;

  /* define callback */
  auto cbk = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
                const char *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (adapter == nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->adapter = adapter;
  };

  /* get adapter */
  wgpuInstanceRequestAdapter(instance, &req_adapter_opts, cbk, (void *)&udata);

  /* return */
  std::cout << "Got adapter: " << udata.adapter << std::endl;

  wgpuInstanceRelease(instance);

  return udata.adapter;
}

WGPUDevice requestWGPUDevice()
{
  WGPUAdapter adapter;
  try {
    adapter = requestWGPUAdapter();
  }
  catch (const std::exception &e) {
    throw e;
  }
  std::cout << "Requesting device..." << std::endl;

  size_t feature_count = wgpuAdapterEnumerateFeatures(adapter, nullptr);
  std::vector<WGPUFeatureName> features(feature_count);
  wgpuAdapterEnumerateFeatures(adapter, features.data());

  WGPUQueueDescriptor queue_descriptor;
  queue_descriptor.label           = nullptr;
  queue_descriptor.nextInChain     = nullptr;

  WGPUDeviceDescriptor descriptor  = {};
  descriptor.requiredFeatures      = features.data();
  descriptor.requiredFeaturesCount = features.size();

  WGPUDeviceDescriptor deviceDesc  = {};
  deviceDesc.label                 = "My Device";
  deviceDesc.requiredFeaturesCount = 0;
  deviceDesc.requiredLimits        = nullptr;
  deviceDesc.defaultQueue.label    = "The default queue";

  struct UserData {
    WGPUDevice device = nullptr;
  } udata;

  auto cbk = [](WGPURequestDeviceStatus status, WGPUDevice device,
                char const *message, void *userdata) {
    UserData *udata_ptr = static_cast<UserData *>(userdata);

    if (device == nullptr) {
      throw std::runtime_error(message);
    }

    udata_ptr->device = device;
  };

  wgpuAdapterRequestDevice(adapter, &deviceDesc, cbk, (void *)&udata);

  std::cout << "Got device: " << udata.device << std::endl;
  wgpuAdapterRelease(adapter);

  return udata.device;
}
