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

#include "ustd/expected.h"

#include <GLFW/glfw3.h>
#include <vector>
#include <webgpu/webgpu_cpp.h>

namespace rndr {

class Globals {
public:
  /**
   * @brief Set the required features. Call this before a call to `initialize`.
   */
  void setRequiredFeatures(std::vector<wgpu::FeatureName> required_features);

  Globals(bool uses_surface = true);

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
  [[nodiscard]] ustd::result initialize(int width = 640, int height = 480);
  bool                       isInitialized();

  const wgpu::Device        &getDevice();
  const wgpu::Limits        &getLimits();
  const ustd::expected<wgpu::Surface>   getSurface();
  const std::vector<wgpu::FeatureName> &getFeatures();
  const wgpu::Queue                    &getQueue();
  GLFWwindow                           *getWindow();

  bool                                  hasFeature(wgpu::FeatureName feature);

  /* returns true if the blocked future is completed */
  bool         blockOnFuture(wgpu::Future future);

  wgpu::Future getSubmittedWorkFuture();
  void         blockOnSubmittedWork();

  void         processEvents();

protected:
  wgpu::Instance instance_ = {};
  wgpu::Device   device_   = {};
  wgpu::Queue    queue_    = {};

  wgpu::Surface  surface_  = {};
  GLFWwindow    *window_   = nullptr;

  /* Features and limits */
  std::vector<wgpu::FeatureName> features_          = {};
  std::vector<wgpu::FeatureName> required_features_ = {};

  wgpu::Limits                   limits_            = {};
  wgpu::RequiredLimits           required_limits_   = {};

  int                            width_             = 0;
  int                            height_            = 0;

private:
  ustd::result initializeWebGPU();
  ustd::result initializeGLFW();

  const bool   uses_surface_;
  bool         initialized_ = false;
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
