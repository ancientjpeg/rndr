/**
 * @file matrix.h
 * @author Jackson Wyatt Kaplan (JwyattK@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RTGPU_MATRIX_H_
#define RTGPU_MATRIX_H_

#include <array>
#include <cassert>
#include <exception>

namespace rtgpu {
namespace math {

template <size_t M, size_t N, typename T = float>
class matrix {
public:
  using transpose_t = matrix<N, M, T>;

  matrix()
  {
    std::fill(data_.begin(), data_.end(), T{});
  }

  matrix(std::initializer_list<T> init)
  {
    assert(init.size() <= size_);
    std::copy(init.begin(), init.end(), data_.begin());
  }

  std::vector<T> to_vector() const
  {
    std::vector<T> ret(size_);

    return ret;
  }

  transpose_t transpose() const
  {
    transpose_t transpose_mat;

    size_t      m = 0, n = 0;
    for (; m < M; ++m) {
      for (; n < N; ++n) {
        transpose_mat.at(n, m) = at(m, n);
      }
    }

    return transpose_mat;
  }

  T &at(size_t m, size_t n)
  {
    assert(m < M && n < N);
    return data_[m * N + n];
  }

  const T &at(size_t m, size_t n) const
  {
    assert(m < M && n < N);
    return data_[m * N + n];
  }

  constexpr size_t size() const
  {
    return size_;
  }

  template <size_t P>
  friend matrix operator*(const matrix<M, P, T> &lhs,
                          const matrix<P, N, T> &rhs)
  {
    matrix ret;
    size_t m, n, p;
    for (m = 0; m < M; ++m) {
      for (n = 0; n < N; ++n) {
        T sum{};
        for (p = 0; p < P; ++p) {
          sum += lhs.at(m, p) * rhs.at(p, n);
        }
        ret.at(m, n) = sum;
      }
    }
    return ret;
  }

private:
  static constexpr size_t size_ = M * N;
  std::array<T, size_>    data_;
};

} // namespace math
} // namespace rtgpu

#endif
