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

#ifndef RNDR_MATRIX_H_
#define RNDR_MATRIX_H_

#include <array>
#include <cassert>

namespace rndr {
namespace math {

template <size_t M, size_t N, typename T = float>
class mat {
public:
  using type                   = T;
  using transpose_t            = mat<N, M, T>;

  static constexpr size_t size = M * N;

  static_assert(size * sizeof(T) < 512);

  mat()
  {
    std::fill(data_.begin(), data_.end(), T{});
  }

  mat(std::initializer_list<T> init)
  {
    assert(init.size() <= size);
    std::copy(init.begin(), init.end(), data_.begin());
  }

  std::vector<T> to_vector() const
  {
    std::vector<T> ret(size);

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

  template <typename = std::enable_if_t<N == 1>>
  T &at(size_t m)
  {
    assert(m < M);
    return data_[m];
  }

  template <typename = std::enable_if_t<N == 1>>
  const T &at(size_t m) const
  {
    assert(m < M);
    return data_[m];
  }

  template <size_t P>
  friend mat operator*(const mat<M, P, T> &lhs, const mat<P, N, T> &rhs)
  {
    mat    ret;
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
  std::array<T, size> data_;
  friend bool         operator==(mat<M, N, T> mat0, mat<M, N, T> mat1);
};

template <size_t M, size_t N, typename T = float>
bool operator==(mat<M, N, T> mat0, mat<M, N, T> mat1)
{
  return mat0.size() == mat1.size()
         && std::memcmp(mat0.data_.data(), mat1.data.data(),
                        sizeof(T) * mat0.size());
}

template <size_t M, typename T = mat<M, 1>::type>
using vec = mat<M, 1, T>;

} // namespace math
} // namespace rndr

#endif
