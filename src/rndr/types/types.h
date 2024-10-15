/**
 * @file types.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_TYPES_H_
#define RNDR_TYPES_H_

#include "rndr/math/matrix.h"

#include <type_traits>
#include <vector>
#include <webgpu/webgpu_cpp.h>

namespace rndr {
/********* Error Type **********/
template <typename T>
struct is_string
    : public std::disjunction<std::is_same<char *, std::decay_t<T>>,
                              std::is_same<const char *, std::decay_t<T>>,
                              std::is_same<std::string, std::decay_t<T>>> {};

struct Error {

  Error(std::string message) : message_(message)
  {
  }

  operator const char *() const
  {
    return message_.data();
  }

private:
  std::string message_;
};

/********** Mesh Data **********/
struct MeshData {
  std::vector<math::vec3>  vertices;
  std::vector<math::zvec3> faces;

  bool                     operator==(const MeshData &other) const
  {
    return vertices == other.vertices && faces == other.faces;
  }
};

/********** Resource Type Traits **********/
template <typename T, typename = void>
struct is_rndr_resource : std::false_type {};

template <typename T>
struct is_rndr_resource<
    T,
    std::void_t<decltype(std::declval<T>().registerWithPipeline(
        std::declval<wgpu::RenderPipeline &>()))>> : std::true_type {};

template <typename T>
static constexpr bool is_rndr_resource_v = is_rndr_resource<T>::value;

/********** Matrix type Traits **********/
/* https://stackoverflow.com/questions/16905359/how-to-check-if-a-type-is-a-specialization-of-the-stdarray-class-template
 */

namespace math {

template <typename>
struct is_basic_tensor : std::false_type {};

template <typename T, size_t... Dims>
struct is_basic_tensor<basic_tensor<T, Dims...>> : std::true_type {};

template <typename T>
static constexpr bool is_basic_tensor_v = is_basic_tensor<T>::value;

static_assert(is_basic_tensor_v<zvec4>);
static_assert(is_basic_tensor_v<basic_scalar<unsigned short>>);
static_assert(is_basic_tensor_v<vec2>);
static_assert(!is_basic_tensor_v<std::vector<float>>);

} // namespace math

} // namespace rndr

#endif
