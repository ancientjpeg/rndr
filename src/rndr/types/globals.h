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

#include <webgpu/webgpu_cpp.h>

#include <vector>

namespace rndr {

class Globals {
public:
  wgpu::Device &getDevice()
  {
    return device_;
  }

  const wgpu::Limits &getLimits()
  {
    return limits_;
  }

  const std::vector<wgpu::FeatureName> &getFeatures()
  {
    return features_;
  }

  bool hasFeature(wgpu::FeatureName feature)
  {
    return std::find(features_.begin(), features_.end(), feature)
           != features_.end();
  }

protected:
  wgpu::Device                   device_   = {};
  wgpu::Limits                   limits_   = {};
  std::vector<wgpu::FeatureName> features_ = {};
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

  wgpu::Device &getDevice()
  {
    return globals_.getDevice();
  }

private:
  Globals &globals_;
};

} // namespace rndr

#endif