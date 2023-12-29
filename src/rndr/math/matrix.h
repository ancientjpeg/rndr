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
#include <tuple>

namespace rndr {
namespace math {

template <typename T, size_t... Dims>
class basic_tensor {
public:
  using scalar_type            = T;
  static constexpr size_t size = (Dims * ...);
  using array_t                = std::array<T, size>;

  static_assert(size * sizeof(T) < (1 << 12));

  basic_tensor()
  {
    std::fill(data_.begin(), data_.end(), T{});
  }

  template <size_t Dim>
  static constexpr size_t dim()
  {
    return std::get<Dim>(std::forward_as_tuple(Dims...));
  }

  basic_tensor(array_t init)
  {
    data_ = init;
  }

  array_t get_data() const
  {
    return data_;
  }

  template <typename... Indices,
            std::enable_if_t<(sizeof...(Indices) == sizeof...(Dims)), bool>
            = true>
  T &at(Indices... indices)
  {
    size_t idx = at_internal(0, indices...);
    return data_[idx];
  }

  template <typename... Indices>
  size_t at_internal(size_t running_idx, size_t idx, Indices... indices)
  {
    /* Needs to be structured like this:
     * - m * M + n for M x N
     * - m * M * N + n * N + p for  M x N x P
     * - m * M * N * P + n * N * P + p * P + q for M x N x P x Q;
     *
     * So, we need to do a schema like this:
     *
     * layer 0: m * M  ....
     * layer 1: ((m * M) + n) * N ...
     * layer 2: ((((m * M) + n) * N) + p) * P ....
     *
     * ...right?
     */

    if constexpr (sizeof...(indices) > 0) {
      constexpr size_t iteration   = sizeof...(Dims) - sizeof...(indices);
      constexpr size_t current_dim = dim<iteration>();
      const size_t     current_idx = (running_idx + idx) * current_dim;
      return at_internal(current_idx, indices...);
    }

    return running_idx + idx;
  }

  constexpr size_t get_size()
  {
    return size;
  }

  friend basic_tensor operator+(const basic_tensor &lhs,
                                const basic_tensor &rhs)
  {
    basic_tensor ret;
    for (size_t i = 0; i < basic_tensor::size; ++i) {
      ret.data_[i] = lhs.data_[i] + rhs.data_[i];
    }
    return ret;
  }

  template <size_t P,
            size_t N,
            size_t DIMS_SIZE                       = sizeof...(Dims),
            std::enable_if_t<DIMS_SIZE == 2, bool> = true>
  basic_tensor<T, basic_tensor::dim<0>(), N>
  operator*(basic_tensor<T, P, N> rhs)
  {
    constexpr auto M = dim<0>();
    static_assert(P == dim<1>());
    basic_tensor<T, M, N> ret;
    size_t                i = 0, j = 0;
    size_t                m, n, p;
    for (m = 0; m < M; ++m) {
      for (n = 0; n < N; ++n) {
        T sum{};
        for (p = 0; p < P; ++p) {
          sum += at(m, p) * rhs.at(p, n);
        }
        ret.at(m, n) = sum;
      }
    }
    return ret;
  }

  /* vector mult*/
  template <size_t P,
            size_t DIMS_SIZE                       = sizeof...(Dims),
            std::enable_if_t<DIMS_SIZE == 2, bool> = true>
  basic_tensor<T, basic_tensor::dim<0>()> operator*(basic_tensor<T, P> rhs)
  {
    constexpr auto M = dim<0>();
    static_assert(P == dim<1>());
    basic_tensor<T, M> ret;
    size_t             i = 0, j = 0;
    size_t             m, n, p;
    for (m = 0; m < M; ++m) {
      T sum{};
      for (p = 0; p < P; ++p) {
        sum += at(m, p) * rhs.at(p);
      }
      ret.at(m) = sum;
    }
    return ret;
  }

  friend bool operator==(const basic_tensor &mat0, const basic_tensor &mat1)
  {
    return std::memcmp(mat0.data_.data(), mat1.data_.data(),
                       sizeof(T) * mat0.size)
           == 0;
  }

  template <size_t DIMS_SIZE                       = sizeof...(Dims),
            std::enable_if_t<DIMS_SIZE == 2, bool> = true>
  basic_tensor transpose()
  {
    decltype(transpose()) transpose_mat;
    constexpr size_t      M = basic_tensor::dim<0>();
    constexpr size_t      N = basic_tensor::dim<1>();
    size_t                m = 0, n = 0;
    for (; m < dim<0>(); ++m) {
      for (; n < dim<1>(); ++n) {
        transpose_mat.at(n, m) = at(m, n);
      }
    }

    return transpose_mat;
  }

  template <size_t M>
  constexpr static basic_tensor<T, M, M> identity()
  {
    basic_tensor<T, M, M> ret;
    for (size_t i = 0; i < M; ++i) {
      ret.at(i, i) = T{1};
    }
    return ret;
  }

private:
  std::array<T, size> data_;
};

/* DEFINITIONS */

template <size_t... Dims>
using tensor = basic_tensor<float, Dims...>;

template <size_t... Dims>
using dtensor = basic_tensor<double, Dims...>;

template <size_t... Dims>
using itensor = basic_tensor<int32_t, Dims...>;

/* MATRIX DEFINITIONS */

template <typename T, size_t M, size_t N>
using basic_matrix = basic_tensor<T, M, N>;

template <size_t M, size_t N>
using matrix = basic_matrix<typename tensor<M>::scalar_type, M, N>;

template <size_t M, size_t N>
using dmatrix = basic_matrix<double, M, N>;

/* VECTOR DEFINITIONS */
template <typename T, size_t M>
using basic_vector = basic_tensor<T, M>;

template <size_t M>
using vector = basic_vector<typename tensor<M>::scalar_type, M>;
using vec2   = vector<2>;
using vec3   = vector<3>;
using vec4   = vector<4>;

template <size_t M>
using ivector = basic_vector<int32_t, M>;
using ivec2   = ivector<2>;
using ivec3   = ivector<3>;
using ivec4   = ivector<4>;

template <size_t M>
using zvector = basic_vector<size_t, M>;
using zvec2   = zvector<2>;
using zvec3   = zvector<3>;
using zvec4   = zvector<4>;

template <size_t M>
using dvector = dtensor<M>;

template <typename T>
using basic_scalar = basic_tensor<T, 1>;
using scalar       = basic_scalar<typename tensor<1>::scalar_type>;
using dscalar      = basic_scalar<double>;
using iscalar      = basic_scalar<int32_t>;
using sscalar      = basic_scalar<size_t>;

/* matrix type traits */
/* https://stackoverflow.com/questions/16905359/how-to-check-if-a-type-is-a-specialization-of-the-stdarray-class-template
 */
template <typename>
struct is_basic_tensor : std::false_type {};

template <typename T, size_t... Dims>
struct is_basic_tensor<basic_tensor<T, Dims...>> : std::true_type {};

} // namespace math
} // namespace rndr

#endif
