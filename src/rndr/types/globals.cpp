/**
 * @file globals.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "rndr/utils/helpers.h"

#include "glfw3webgpu.h"
#include "globals.h"

#include <future>
#include <iostream>

namespace rndr {
using namespace wgpu;

Globals::~Globals()
{
  glfwDestroyWindow(getWindow());
  glfwTerminate();
}

/* PRE-INIT SETTERS */
void Globals::setRequiredFeatures(std::vector<FeatureName> required_features)
{
  required_features_ = std::move(required_features);
}

void Globals::setRequiredLimits(Limits required_limits)
{
  required_limits_.limits = std::move(required_limits);
}

void Globals::initialize(int width, int height)
{

  width_  = width;
  height_ = height;

  /* GLFW init */
  glfwInitHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(width_, height_, "RNDR", nullptr, nullptr);

  /* Instance + surface init */
  InstanceDescriptor instance_desc = {};
  instance_                        = CreateInstance();

  surface_ = Surface::Acquire(glfwGetWGPUSurface(instance_.Get(), window_));

  /* Request adapter */
  RequestAdapterOptions adapter_opts = {};
  adapter_opts.powerPreference       = PowerPreference::HighPerformance;
  adapter_opts.compatibleSurface     = surface_;

  std::promise<WGPUAdapter> adapter_req_promise;
  std::future<WGPUAdapter>  adapter_req_future
      = adapter_req_promise.get_future();

  auto adapter_req_callback = [](WGPURequestAdapterStatus status,
                                 WGPUAdapter adapter, char const *message,
                                 void *userdata) {
    if (status != WGPURequestAdapterStatus_Success) {
      std::cerr << "ADAPTER REQUEST FAILED WITH MESSAGE: " << message
                << std::endl;
    }
    else {
      static_cast<std::promise<WGPUAdapter> *>(userdata)->set_value(adapter);
    }
  };

  instance_.RequestAdapter(&adapter_opts, adapter_req_callback,
                           &adapter_req_promise);

  wgpu::Adapter adapter;

  try {
    adapter = Adapter::Acquire(adapter_req_future.get());
    std::cout << "Successfully acquired adapter at address:" << adapter.Get()
              << std::endl;
  }
  catch (std::exception &e) {
    std::cerr << "Adapter request failed with following message: " << e.what()
              << std::endl;
    throw e;
  }

  /* Get features */
  features_.resize(adapter.EnumerateFeatures(nullptr));
  adapter.EnumerateFeatures(features_.data());

  std::sort(features_.begin(), features_.end());
  std::sort(required_features_.begin(), required_features_.end());

  for (FeatureName feature : required_features_) {
    if (std::find(features_.begin(), features_.end(), feature)
        == features_.end()) {
      throw std::runtime_error("Feature with code"
                               + std::to_string((int)feature) + "unavailable");
    }
  }

  /* Test limits */
  SupportedLimits supported_limits = {};
  adapter.GetLimits(&supported_limits);

  if (!helpers::limits_supported(supported_limits.limits,
                                 required_limits_.limits)) {
    throw std::runtime_error("Cannot support required limits");
  }

  /* Request device */
  DeviceDescriptor device_desc     = {};
  device_desc.requiredFeatures     = features_.data();
  device_desc.requiredFeatureCount = features_.size();
  device_desc.requiredLimits       = &required_limits_;
  device_desc.label                = "rndr Application Device";

  std::promise<WGPUDevice> device_req_promise;
  std::future<WGPUDevice>  device_req_future = device_req_promise.get_future();

  adapter.RequestDevice(
      &device_desc,
      [](WGPURequestDeviceStatus status, WGPUDevice device, char const *message,
         void *userdata) {
        if (status != WGPURequestDeviceStatus_Success) {
          throw std::runtime_error(message);
        }
        static_cast<std::promise<WGPUDevice> *>(userdata)->set_value(device);
      },
      &device_req_promise);

  try {
    device_ = Device::Acquire(device_req_future.get());
    std::cout << "Successfully acquired device at address:" << device_.Get()
              << std::endl;
  }
  catch (std::exception &e) {
    std::cerr << "Device request failed with following message: " << e.what()
              << std::endl;
    throw e;
  }

  /* set default callbacks */
  device_.SetDeviceLostCallback(helpers::default_device_lost_callback, nullptr);
  device_.SetUncapturedErrorCallback(helpers::default_error_callback, nullptr);

  /* get swap chain */
  SwapChainDescriptor sc_desc = {};
  sc_desc.width               = width_;
  sc_desc.height              = height_;
  sc_desc.format              = wgpu::TextureFormat::BGRA8Unorm;
  sc_desc.usage               = wgpu::TextureUsage::RenderAttachment;
  sc_desc.presentMode         = wgpu::PresentMode::Fifo;
  sc_desc.label               = "Main SwapChain";

  swap_chain_  = std::move(device_.CreateSwapChain(surface_, &sc_desc));

  queue_       = getDevice().GetQueue();

  initialized_ = true;
}

bool Globals::isInitialized()
{
  return initialized_;
}

const wgpu::Device &Globals::getDevice()
{
  return device_;
}

const wgpu::Queue &Globals::getQueue()
{
  return queue_;
}

const wgpu::SwapChain &Globals::getSwapChain()
{
  return swap_chain_;
}

GLFWwindow *Globals::getWindow()
{
  return window_;
}

const wgpu::Limits &Globals::getLimits()
{
  return limits_;
}

const wgpu::Surface &Globals::getSurface()
{
  return surface_;
}

const std::vector<wgpu::FeatureName> &Globals::getFeatures()
{
  return features_;
}

bool Globals::hasFeature(wgpu::FeatureName feature)
{
  return std::find(features_.begin(), features_.end(), feature)
         != features_.end();
}

} // namespace rndr
