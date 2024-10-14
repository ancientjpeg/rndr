/**
 * @file globals.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_GLOBALS_H_
#define RNDR_GLOBALS_H_

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <vector>

namespace rndr {

class Globals {
public:
  /**
   * @brief Set the required features. Call this before a call to `initialize`.
   */
  void setRequiredFeatures(std::vector<wgpu::FeatureName> required_features);

  virtual ~Globals();

  /**
   * @brief Set the required limits. Call this before a call to `initialize`.
   */
  void setRequiredLimits(wgpu::Limits required_limits);

  /**
   * @brief Initialize all global WebGPU and GLFW objects.
   *
   * @note `initialize()` can throw.
   * @todo make this nothrow - why did i use exceptions ?
   *
   * @param width Initial width of the screen, in pixels
   * @param height Initial height of the screen, in pixels
   */
  void                 initialize(int width = 640, int height = 480);
  bool                 isInitialized();

  const wgpu::Device  &getDevice();
  const wgpu::Limits  &getLimits();
  const wgpu::Surface &getSurface();
  const std::vector<wgpu::FeatureName> &getFeatures();
  const wgpu::Queue                    &getQueue();
  const wgpu::SwapChain                &getSwapChain();
  GLFWwindow                           *getWindow();

  bool                                  hasFeature(wgpu::FeatureName feature);

protected:
  wgpu::Limits                   limits_     = {};

  wgpu::Instance                 instance_   = {};
  wgpu::Device                   device_     = {};
  wgpu::Queue                    queue_      = {};

  wgpu::Surface                  surface_    = {};
  GLFWwindow                    *window_     = nullptr;
  wgpu::SwapChain                swap_chain_ = {};

  std::vector<wgpu::FeatureName> features_   = {};

  /* Features and limits */
  std::vector<wgpu::FeatureName> required_features_ = {};
  wgpu::RequiredLimits           required_limits_   = {};

  int                            width_             = 0;
  int                            height_            = 0;

  bool                           initialized_       = false;
};

class GlobalAccess {

public:
  GlobalAccess(Globals &globals) : globals_(globals)
  {
  }

protected:
  Globals &getGlobals()
  {
    return globals_;
  }

  const wgpu::Device &getDevice()
  {
    return globals_.getDevice();
  }

private:
  Globals &globals_;
};

} // namespace rndr

#endif
