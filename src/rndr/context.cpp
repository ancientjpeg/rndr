/**
 * @file context.cpp
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-10-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "rndr/utils/helpers.h"

#include "context.h"
#include "glfw3webgpu.h"

#include <cassert>
#include <chrono>
#include <iostream>

namespace rndr {

Context::Context(bool uses_surface) : uses_surface_(uses_surface)
{
}

Context::~Context()
{
  /* manually release wgpu surface-related assets */
  if (surface_) {
    wgpuSurfaceRelease(surface_->MoveToCHandle());
  }

  if (window_) {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  if (queue_) {
    wgpuQueueRelease(queue_.MoveToCHandle());
  }
  if (device_) {
    wgpuDeviceRelease(device_.MoveToCHandle());
  }
  if (instance_) {
    instance_.ProcessEvents();
    wgpuInstanceRelease(instance_.MoveToCHandle());
  }
}

/* PRE-INIT SETTERS */
void Context::setRequiredFeatures(
    std::vector<wgpu::FeatureName> required_features)
{
  required_features_ = std::move(required_features);
}

void Context::setRequiredLimits(wgpu::Limits required_limits)
{
  required_limits_.limits = std::move(required_limits);
}

ustd::result Context::initializeWebGPU()
{
  wgpu::InstanceDescriptor instance_desc      = {};
  instance_desc.features.timedWaitAnyEnable   = true;
  instance_desc.features.timedWaitAnyMaxCount = 8;
  instance_                                   = CreateInstance(&instance_desc);

  if (instance_.Get() == nullptr) {
    return ustd::unexpected("Failed to retrieve instance");
  }

  if (uses_surface_) {
    surface_
        = wgpu::Surface::Acquire(glfwGetWGPUSurface(instance_.Get(), window_));

    if (surface_->Get() == nullptr) {
      return ustd::unexpected("Failed to retrieve surface");
    }
  }

  /* Request adapter */
  wgpu::RequestAdapterOptions adapter_opts = {};
  adapter_opts.powerPreference   = wgpu::PowerPreference::HighPerformance;
  adapter_opts.compatibleSurface = surface_ ? *surface_ : nullptr;

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
    return ustd::unexpected("Failed to retrieve adapter");
  }

  /* Get features */
  features_.resize(adapter_.EnumerateFeatures(nullptr));
  adapter_.EnumerateFeatures(features_.data());

  std::sort(features_.begin(), features_.end());
  std::sort(required_features_.begin(), required_features_.end());

  for (wgpu::FeatureName feature : required_features_) {
    if (std::find(features_.begin(), features_.end(), feature)
        == features_.end()) {
      return ustd::unexpected("Feature with code" + std::to_string((int)feature)
                              + "unavailable");
    }
  }

  /* Test limits */
  wgpu::SupportedLimits supported_limits = {};
  adapter_.GetLimits(&supported_limits);

  if (!helpers::limits_supported(supported_limits.limits,
                                 required_limits_.limits)) {
    return ustd::unexpected("Cannot support required limits");
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
    return ustd::unexpected("Failed to retrieve device");
  }

  if (!error_msg.empty()) {
    return ustd::unexpected(error_msg);
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

  if (uses_surface_) {
    surface_->Configure(&surface_config);
  }

  return {};
}

ustd::result Context::initializeGLFW()
{
  /* GLFW init */
  glfwInitHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(width_, height_, "RNDR", nullptr, nullptr);
  if (window_ == nullptr) {
    return ustd::unexpected("Failed to create GLFW window");
  }

  return {};
}

ustd::result Context::initialize(int width, int height)
{
  width_  = width;
  height_ = height;

  if (uses_surface_) {
    if (auto result = initializeGLFW(); !result) {
      return result;
    }
  }

  if (auto result = initializeWebGPU(); !result) {
    return result;
  }

  initialized_ = true;

  return {};
}

bool Context::blockOnFuture(wgpu::Future future)
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

wgpu::Future Context::getSubmittedWorkFuture()
{
  assert(isInitialized());
  wgpu::Future future = getQueue().OnSubmittedWorkDone(
      wgpu::CallbackMode::AllowProcessEvents,
      [](wgpu::QueueWorkDoneStatus status) {
        assert(status == wgpu::QueueWorkDoneStatus::Success);
      });

  return future;
}

void Context::blockOnSubmittedWork()
{
  blockOnFuture(getSubmittedWorkFuture());
}

void Context::processEvents()
{
  instance_.ProcessEvents();
}

bool Context::isInitialized()
{
  return initialized_;
}

const wgpu::Device &Context::getDevice()
{
  return device_;
}

const wgpu::Queue &Context::getQueue()
{
  return queue_;
}

GLFWwindow *Context::getWindow()
{
  return window_;
}

const wgpu::Limits &Context::getLimits()
{
  return limits_;
}

const ustd::expected<wgpu::Surface> Context::getSurface()
{
  if (!uses_surface_) {
    return ustd::unexpected("Context was not configured to use a surface.");
  }
  return *surface_;
}

const std::vector<wgpu::FeatureName> &Context::getFeatures()
{
  return features_;
}

bool Context::hasFeature(wgpu::FeatureName feature)
{
  return std::find(features_.begin(), features_.end(), feature)
         != features_.end();
}

} // namespace rndr
