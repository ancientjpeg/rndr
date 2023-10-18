/**
 * @file application.cpp
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "application.h"
#include "glfw3webgpu.h"
#include "rndr/utils/helpers.h"
#include <cassert>
#include <fstream>
#include <future>
#include <iostream>
#include <sstream>

using std::literals::string_literals::operator""s;

namespace rndr {

using namespace wgpu;

Application::Application()
    : support_dir_(RNDR_SUPPORT_DIR), shader_dir_(support_dir_ / "shaders")
{
  assert(support_dir_.is_absolute() && std::filesystem::exists(support_dir_));
  assert(shader_dir_.is_absolute() && std::filesystem::exists(shader_dir_));

  /** @todo DELETE! */
  collectShaderSource_(true);
}

/* PRE-INIT SETTERS */
void Application::setRequiredFeatures(
    std::vector<FeatureName> required_features)
{
  required_features_ = std::move(required_features);
}

void Application::setRequiredLimits(Limits required_limits)
{
  required_limits_.limits = std::move(required_limits);
}

/* INITIALIZE */
void Application::initialize(int width, int height)
{
  width_  = width;
  height_ = height;

  /* GLFW init */
  glfwInitHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(width_, height_, "RNDR", nullptr, nullptr);

  collectShaderSource_();

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

  try {
    adapter_ = Adapter::Acquire(adapter_req_future.get());
    std::cout << "Successfully acquired adapter at address:" << adapter_.Get()
              << std::endl;
  }
  catch (std::exception &e) {
    std::cerr << "Adapter request failed with following message: " << e.what()
              << std::endl;
    throw e;
  }

  /* Check features */
  std::vector<FeatureName> available_features(
      adapter_.EnumerateFeatures(nullptr));
  adapter_.EnumerateFeatures(available_features.data());

  std::sort(available_features.begin(), available_features.end());
  std::sort(required_features_.begin(), required_features_.end());

  for (FeatureName feature : required_features_) {
    if (std::find(available_features.begin(), available_features.end(), feature)
        == available_features.end()) {
      throw std::runtime_error("Feature with code"s
                               + std::to_string((int)feature) + "unavailable"s);
    }
  }

  /* Test limits */
  SupportedLimits supported_limits = {};
  adapter_.GetLimits(&supported_limits);

  if (!helpers::limits_supported(supported_limits.limits,
                                 required_limits_.limits)) {
    throw std::runtime_error("Cannot support required limits");
  }

  /* Request device */
  DeviceDescriptor device_desc      = {};
  device_desc.requiredFeatures      = available_features.data();
  device_desc.requiredFeaturesCount = available_features.size();
  device_desc.requiredLimits        = &required_limits_;
  device_desc.label                 = "rndr Application Device";

  std::promise<WGPUDevice> device_req_promise;
  std::future<WGPUDevice>  device_req_future = device_req_promise.get_future();

  adapter_.RequestDevice(
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

  swap_chain_ = std::move(device_.CreateSwapChain(surface_, &sc_desc));
}

bool Application::addShaderSource(std::filesystem::path shader_path)
{
  using namespace std::filesystem;

  if (!exists(shader_path) || shader_path.extension() != ".wgsl") {
    return false;
  }

  std::stringstream out;
  std::ifstream     ifs(shader_path);
  out << ifs.rdbuf();

  auto name = shader_path.string().substr(shader_dir_.string().size() + 1);
  if (shader_code_.count(name) != 0) {
    throw std::runtime_error("DUPLICATE SHADER");
    return false;
  }
  shader_code_[name] = {name, shader_path, out.str()};
  return true;
}

void Application::collectShaderSource_(bool rescan)
{

  using namespace std::filesystem;

  if (rescan) {
    shader_code_.clear();
  };

  for (auto const &dir_entry : recursive_directory_iterator{shader_dir_}) {
    auto p = dir_entry.path();
    addShaderSource(p);
  }
}

} // namespace rndr