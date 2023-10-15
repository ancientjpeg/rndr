/**
 * @file math.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RNDR_MATH_H_
#define RNDR_MATH_H_

#include <numeric>

namespace rndr {
namespace math {

template <typename T, typename... Scalars>
T length(Scalars... scalars)
{
  constexpr size_t      el_cnt = sizeof...(Scalars);
  std::array<T, el_cnt> els{static_cast<T>(scalars)...};

  for (size_t i = 0; i < el_cnt; ++i) {
    els[i] *= els[i];
  }

  T sum = std::accumulate(els.begin(), els.end(), static_cast<T>(0));
  return std::sqrt(sum);
}

} // namespace math
} // namespace rndr

#endif