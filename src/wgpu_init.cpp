#include "wgpu_init.h"
#include "glfw3webgpu.h"
#include <cassert>
#include <exception>
#include <iostream>
#include <vector>

void wgpu_error_callback(WGPUErrorType type, char const *message, void *)
{
  std::cerr << "WGPU ERROR TYPE: " << type << std::endl;
  std::cerr << "WGPU ERROR MESSAGE: " << message << std::endl << std::endl;
}

void wgpu_device_lost_callback(WGPUDeviceLostReason reason,
                               char const          *message,
                               void *)
{
  std::cerr << "WGPU DEVICE LOST REASON: " << reason << std::endl;
  std::cerr << "WGPU DEVICE LOST MESSAGE: " << message << std::endl
            << std::endl;
}

WGPUAdapter requestWGPUAdapter(WGPUInstance instance)
{
  std::cout << "Requesting adapter..." << std::endl;

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

  return udata.adapter;
}

WGPUDevice requestWGPUDevice(WGPUAdapter adapter)
{
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
  deviceDesc.label                 = "WGPU Default Device";
  deviceDesc.requiredFeaturesCount = 0;
  deviceDesc.requiredLimits        = nullptr;
  deviceDesc.defaultQueue.label    = "WGPU Default Queue";

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

  return udata.device;
}

WGPUGlobals requestWGPUGlobals()
{

  WGPUGlobals globals;

  /* get instance */
  WGPUInstanceDescriptor instance_desc = {};
  instance_desc.nextInChain            = nullptr;

  globals.instance                     = wgpuCreateInstance(&instance_desc);
  globals.adapter                      = requestWGPUAdapter(globals.instance);
  globals.device                       = requestWGPUDevice(globals.adapter);

  wgpuDeviceSetUncapturedErrorCallback(globals.device, wgpu_error_callback,
                                       nullptr);
  wgpuDeviceSetDeviceLostCallback(globals.device, wgpu_device_lost_callback,
                                  nullptr);

  return globals;
}

void wgpuGlobalsRelease(WGPUGlobals globals)
{
  wgpuDeviceRelease(globals.device);
  wgpuAdapterRelease(globals.adapter);
  wgpuInstanceRelease(globals.instance);
}