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

#include <cassert>
#include <chrono>
#include <iostream>

namespace rndr {

Globals::~Globals()
{
  /* manually release wgpu surface-related assets */
  wgpuSurfaceRelease(surface_.MoveToCHandle());

  glfwDestroyWindow(getWindow());
  glfwTerminate();

  wgpuQueueRelease(queue_.MoveToCHandle());
  wgpuDeviceRelease(device_.MoveToCHandle());
  instance_.ProcessEvents();
  wgpuInstanceRelease(instance_.MoveToCHandle());
}

/* PRE-INIT SETTERS */
void Globals::setRequiredFeatures(
    std::vector<wgpu::FeatureName> required_features)
{
  required_features_ = std::move(required_features);
}

void Globals::setRequiredLimits(wgpu::Limits required_limits)
{
  required_limits_.limits = std::move(required_limits);
}

Result Globals::initializeWebGPU()
{
  wgpu::InstanceDescriptor instance_desc      = {};
  instance_desc.features.timedWaitAnyEnable   = true;
  instance_desc.features.timedWaitAnyMaxCount = 8;
  instance_                                   = CreateInstance(&instance_desc);

  if (instance_.Get() == nullptr) {
    return Result::error("Failed to retrieve instance");
  }

  surface_
      = wgpu::Surface::Acquire(glfwGetWGPUSurface(instance_.Get(), window_));

  if (surface_.Get() == nullptr) {
    return Result::error("Failed to retrieve surface");
  }

  /* Request adapter */
  wgpu::RequestAdapterOptions adapter_opts = {};
  adapter_opts.powerPreference   = wgpu::PowerPreference::HighPerformance;
  adapter_opts.compatibleSurface = surface_;

  wgpu::Adapter adapter_;

  auto          adapter_req_callback
      = [&adapter_](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
                    char const *message) {
          if (status != wgpu::RequestAdapterStatus::Success) {
            std::cerr << "ADAPTER REQUEST FAILED WITH MESSAGE: " << message
                      << std::endl;
          }
          adapter_ = std::move(adapter);
        };

  wgpu::Future adapter_wait_future = instance_.RequestAdapter(
      &adapter_opts, wgpu::CallbackMode::AllowProcessEvents,
      adapter_req_callback);
  blockOnFuture(adapter_wait_future);

  if (adapter_.Get() == nullptr) {
    return Result::error("Failed to retrieve adapter");
  }

  /* Get features */
  features_.resize(adapter_.EnumerateFeatures(nullptr));
  adapter_.EnumerateFeatures(features_.data());

  std::sort(features_.begin(), features_.end());
  std::sort(required_features_.begin(), required_features_.end());

  for (wgpu::FeatureName feature : required_features_) {
    if (std::find(features_.begin(), features_.end(), feature)
        == features_.end()) {
      return Result::error("Feature with code" + std::to_string((int)feature)
                           + "unavailable");
    }
  }

  /* Test limits */
  wgpu::SupportedLimits supported_limits = {};
  adapter_.GetLimits(&supported_limits);

  if (!helpers::limits_supported(supported_limits.limits,
                                 required_limits_.limits)) {
    return Result::error("Cannot support required limits");
  }

  /* Request device */
  wgpu::DeviceDescriptor device_desc = {};
  device_desc.requiredFeatures       = features_.data();
  device_desc.requiredFeatureCount   = features_.size();
  device_desc.requiredLimits         = &required_limits_;
  device_desc.label                  = "rndr Application Device";

  wgpu::DeviceLostCallbackInfo lost_cb_info;
  lost_cb_info.callback              = helpers::default_device_lost_callback;
  lost_cb_info.mode                  = wgpu::CallbackMode::AllowProcessEvents;

  device_desc.deviceLostCallbackInfo = lost_cb_info;

  std::string  error_msg;
  wgpu::Future device_future = adapter_.RequestDevice(
      &device_desc, wgpu::CallbackMode::AllowProcessEvents,
      [&](wgpu::RequestDeviceStatus status, wgpu::Device device,
          char const *message) {
        if (status != wgpu::RequestDeviceStatus::Success) {
          error_msg = message;
        }
        device_ = std::move(device);
      });

  blockOnFuture(device_future);

  if (device_.Get() == nullptr) {
    return Result::error("Failed to retrieve device");
  }

  if (!error_msg.empty()) {
    return Result::error(error_msg);
  }

  /* set default callbacks */
  device_.SetUncapturedErrorCallback(helpers::default_error_callback, nullptr);

  queue_ = getDevice().GetQueue();

  /* configure surface */
  wgpu::SurfaceConfiguration surface_config;
  surface_config.device      = device_;
  surface_config.format      = wgpu::TextureFormat::BGRA8Unorm;
  surface_config.width       = width_;
  surface_config.height      = height_;
  surface_config.presentMode = wgpu::PresentMode::Fifo;

  surface_.Configure(&surface_config);

  return Result::success();
}

Result Globals::initializeGLFW()
{
  /* GLFW init */
  glfwInitHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(width_, height_, "RNDR", nullptr, nullptr);
  if (window_ == nullptr) {
    return Result::error("Failed to create GLFW window");
  }

  return Result::success();
}

Result Globals::initialize(int width, int height)
{
  width_  = width;
  height_ = height;

  if (auto result = initializeGLFW(); !result) {
    return result;
  }

  if (auto result = initializeWebGPU(); !result) {
    return result;
  }

  initialized_ = true;

  return Result::success();
}

bool Globals::blockOnFuture(wgpu::Future future)
{
  assert(instance_.Get() != nullptr);
  constexpr auto       timeout    = std::chrono::milliseconds(100);
  constexpr auto       timeout_ns = std::chrono::nanoseconds(timeout).count();

  wgpu::FutureWaitInfo wait_info{future};
  wgpu::WaitStatus wait_status  = instance_.WaitAny(1, &wait_info, timeout_ns);
  bool             wait_success = wait_status == wgpu::WaitStatus::Success;
  assert(wait_success);
  return wait_info.completed && wait_success;
}

wgpu::Future Globals::getSubmittedWorkFuture()
{
  assert(isInitialized());
  wgpu::Future future = getQueue().OnSubmittedWorkDone(
      wgpu::CallbackMode::AllowProcessEvents,
      [](wgpu::QueueWorkDoneStatus status) {
        assert(status == wgpu::QueueWorkDoneStatus::Success);
      });

  return future;
}

void Globals::blockOnSubmittedWork()
{
  blockOnFuture(getSubmittedWorkFuture());
}

void Globals::processEvents()
{
  instance_.ProcessEvents();
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
