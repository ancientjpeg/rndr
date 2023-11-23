/**
 * @file uniform.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1t
 * @date 2023-10-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_UNIFORM_H_
#define RNDR_UNIFORM_H_

#include "rndr/math/matrix.h"
#include "webgpu/webgpu_cpp.h"

namespace rndr {

template <typename T = math::scalar>
class Uniform {
  static_assert(math::is_basic_tensor<T>::value);

private:
  wgpu::Buffer buffer_;
};

} // namespace rndr

#endif