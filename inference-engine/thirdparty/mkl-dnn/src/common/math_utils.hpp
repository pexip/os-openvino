/*******************************************************************************
* Copyright 2017-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef COMMON_MATH_UTILS_HPP
#define COMMON_MATH_UTILS_HPP

#include <math.h>
#include <stdint.h>

#include "dnnl_traits.hpp"
#include "nstl.hpp"
#include "utils.hpp"

#include "../cpu/platform.hpp"

namespace dnnl {
namespace impl {
namespace math {

inline int gcd(int a, int b) {
    a = impl::nstl::abs(a);
    b = impl::nstl::abs(b);
    if (a < b) {
        int x = a;
        a = b;
        b = x;
    }

    if (b == 0) return a;

    int r;
    while ((r = a % b) != 0) {
        a = b;
        b = r;
    }

    return b;
}

template <typename T>
inline bool is_pow2(const T &v) {
    return (v != 0) && ((v & (v - 1)) == 0);
}

/** returns floor(log2(v)), aka the position of the leftmost non-0 bit */
inline int ilog2q(size_t v) {
    if (v == 0) return -1;

    int p = 0;
#define CP(pw) \
    do { \
        if (v >= (1ull << pw)) { \
            v >>= pw; \
            p += pw; \
        } \
    } while (0)
    CP(32);
    CP(16);
    CP(8);
    CP(4);
    CP(2);
    CP(1);
#undef CP
    return p;
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U one_m_square(T x) {
    return (U)(1 - x) * (1 + x);
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U x_m_square(T x) {
    return (U)(1 - x) * x;
}

/* activation */

/** rounds @p f to an integer according to the mxcsr register */
inline float mxcsr_round(float f) ATTR_NO_MSAN {
    return nearbyintf(f);
}

/** converts @p f to an integer according to the mxcsr register */
inline int mxcsr_cvt(float f) ATTR_NO_MSAN {
    return (int)mxcsr_round(f);
}

inline float round_fwd(float s) {
    return mxcsr_round(s);
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline typename utils::enable_if<nstl::is_integral<U>::value, U>::type relu_fwd(
        T s, A alpha) {
    return s > 0 ? s : (U)mxcsr_cvt(static_cast<float>(s * alpha));
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline typename utils::enable_if<!nstl::is_integral<U>::value, U>::type
relu_fwd(T s, A alpha) {
    return s > 0 ? s : (U)(s * alpha);
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U relu_bwd(T dd, T s, A alpha) {
    return s > 0 ? dd : (U)(dd * alpha);
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U relu_bwd(T s, A alpha) {
    return s > 0 ? (U)1 : (U)alpha;
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U relu_bwd_use_dst(T dd, T d, A alpha) {
    return d > 0 ? dd : (U)(dd * alpha);
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U tanh_fwd(T s) {
    const float e = tanhf((float)s);
    return (U)e;
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U tanh_bwd(T dd, T s) {
    const float e = tanh_fwd<float>((float)s);
    return (U)(dd * (1 - e) * (1 + e));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U tanh_bwd_use_dst(T dd, T d) {
    return (U)(dd * (1 - d) * (1 + d));
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U elu_fwd(T s, A alpha) {
    return s > 0 ? s : (U)(alpha * (::expm1f((float)s)));
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U elu_bwd(T dd, T s, A alpha) {
    return (U)(dd * (s > 0 ? 1 : alpha * ::expf((float)s)));
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U elu_bwd_use_dst(T dd, T d, A alpha) {
    return (U)(dd * (d > 0 ? 1 : d + alpha));
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U square_fwd(T s) {
    return s * s;
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U square_bwd(T dd, T s) {
    return dd * 2 * s;
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U abs_fwd(T s) {
    return s > 0 ? s : (U)-s;
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U abs_bwd(T dd, T s) {
    return s > 0 ? dd : s < 0 ? (U)-dd : (U)0;
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U sqrt_fwd(T s) {
    return (U)(::sqrtf((float)(s)));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U sqrt_bwd(T dd, T s) {
    return (U)(dd / (2 * ::sqrtf((float)(s))));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U sqrt_bwd_use_dst(T dd, T d) {
    return (U)(dd / (2 * d));
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U linear_fwd(T s, A alpha, A beta) {
    return (U)(alpha * s + beta);
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U linear_bwd(T dd, T s, A alpha, A beta) {
    (void)s;
    (void)beta;
    return (U)(dd * alpha);
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U bounded_relu_fwd(T s, A alpha) {
    s = s > 0 ? s : (U)0;
    return s > alpha ? (U)(alpha) : s;
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U bounded_relu_bwd(T dd, T s, A alpha) {
    return dd * (0 < s && s <= alpha ? 1 : 0);
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U logistic_fwd(T s) {
    // Here we avoid division/inverse by infinity as some architectures have
    // non-standard behavior
    float exp_overflow_bound = 88.72283172607421875;
    float in = (float)-s;
    return in < exp_overflow_bound ? (U)(1.f / (1.f + ::expf(in))) : 0.f;
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U logistic_bwd(T dd, T s) {
    float v = logistic_fwd<float>(s);
    return (U)(dd * v * (1 - v));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U logistic_bwd_use_dst(T dd, T d) {
    return (U)(dd * d * (1 - d));
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U soft_relu_fwd(T s) {
    float exp_overflow_bound = 20.f;
    float in = (float)s;
    return in < exp_overflow_bound ? (U)(::log1pf(::expf(in))) : (U)in;
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U soft_relu_bwd(T dd, T s) {
    return (U)(dd * logistic_fwd<float>(s));
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U swish_fwd(T s, A alpha) {
    return (U)(s * logistic_fwd<float>(alpha * s));
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U swish_bwd(T dd, T s, A alpha) {
    float v = logistic_fwd<float>(alpha * s);
    return dd * (v + s * alpha * v * (1 - v));
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U exp_fwd(T s) {
    return (U)(::expf((float)s));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U exp_bwd(T dd, T s) {
    return (U)(dd * (::expf((float)s)));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U exp_bwd_use_dst(T dd, T d) {
    return (U)(dd * d);
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U gelu_tanh_fwd(T s) {
    const float sqrt_2_over_pi = 0.79788458347320556640625f;
    const float fitting_const = 0.044715f;
    float v = tanh_fwd(sqrt_2_over_pi * s * (1 + fitting_const * s * s));
    return (U)(0.5 * s * (1. + v));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U gelu_tanh_bwd(T dd, T s) {
    const float sqrt_2_over_pi = 0.79788458347320556640625f;
    const float fitting_const = 0.044715f;
    float g = s * sqrt_2_over_pi * (1 + fitting_const * s * s);
    float dg = sqrt_2_over_pi * (1 + 3 * fitting_const * s * s);
    float v = tanh_fwd(g);
    return (U)(dd * 0.5 * (1. + v) * (1. + s * (1 - v) * dg));
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U log_fwd(T s) {
    return (U)(::logf((float)s));
}
template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U log_bwd(T dd, T s) {
    return (U)(dd * (1.f / (float)s));
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U clip_fwd(T s, A alpha, A beta) {
    s = s > alpha ? s : (U)alpha;
    return s > beta ? (U)beta : s;
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U clip_bwd(T dd, T s, A alpha, A beta) {
    return dd * (alpha < s && s <= beta ? 1 : 0);
}

template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U pow_fwd(T s, A alpha, A beta) {
    return (U)(alpha * ::powf((float)s, beta));
}
template <typename T, typename A,
        typename U = typename utils::remove_reference<T>::type>
inline U pow_bwd(T dd, T s, A alpha, A beta) {
    if (beta == 0) return 0;

    float v = pow_fwd(s, alpha * beta, beta - 1);
    return (U)(dd * v);
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U gelu_erf_fwd(T s) {
    const float sqrt_2_over_2 = 0.707106769084930419921875f;
    float v = s * sqrt_2_over_2;
    return (U)(0.5f * s * (1.f + ::erff(v)));
}

template <typename T, typename U = typename utils::remove_reference<T>::type>
inline U gelu_erf_bwd(T dd, T s) {
    const float two_over_sqrt_pi = 1.12837922573089599609375f;
    const float sqrt_2_over_2 = 0.707106769084930419921875f;
    float v = s * sqrt_2_over_2;
    return (U)(dd * 0.5f
            * (1.f + ::erff(v) + v * two_over_sqrt_pi * ::expf(-v * v)));
}

template <typename T,
        typename U = typename utils::remove_reference<T>::type>
inline U hswish_fwd(T s) {
    float v = s + 3.0f;
    v = v > 0.0f ? v : 0.0f;
    v = v < 6.0f ? v : 6.0f;
    return (U)(v / 6.0f * s);
}

template <typename T,
        typename U = typename utils::remove_reference<T>::type>
inline U mish_fwd(T s) {
    float threshold = 20.f;
    float in = (float)s;
    float v = ::expf(in) * (::expf(in) + 2.f);
    float result = (in * v) / (v + 2.f);
    return in < threshold ? (U)(result) : (U)(in);
}

template <typename T,
        typename U = typename utils::remove_reference<T>::type>
inline U hsigmoid_fwd(T s) {
    float v = s + 3.0f;
    v = v > 0.0f ? v : 0.0f;
    v = v < 6.0f ? v : 6.0f;
    return (U)(v / 6.0f);
}

template <typename T,
        typename U = typename utils::remove_reference<T>::type>
inline U round_half_to_even_fwd(T s) {
    float r = ::roundf((float)s);
    float d = (float)s - r;
    float remainder = ::fmodf(r, 2.0f);
    return ((d != 0.5f) && (d != -0.5f)) || (remainder == 0.0f) ? (U)r :
           (U)((float)s + d);
}

template <typename T,
        typename U = typename utils::remove_reference<T>::type>
inline U round_half_away_from_zero_fwd(T s) {
    return (U)(::roundf((float)s));
}

inline bool is_eltwise_ok(
        data_type_t dt, alg_kind_t alg, float alpha, float beta) {
    using namespace alg_kind;
    using namespace utils;

    const bool eltwise_use_src
            = one_of(alg, eltwise_relu, eltwise_tanh, eltwise_elu,
                      eltwise_square, eltwise_abs, eltwise_sqrt, eltwise_linear,
                      eltwise_bounded_relu, eltwise_soft_relu, eltwise_logistic,
                      eltwise_exp, eltwise_gelu_tanh, eltwise_swish,
                      eltwise_log, eltwise_clip, eltwise_pow, eltwise_gelu_erf,
                      eltwise_round, eltwise_mish, eltwise_hswish, eltwise_hsigmoid,
                      eltwise_round_half_away_from_zero, eltwise_round_half_to_even)
            && IMPLICATION(alg == eltwise_bounded_relu, alpha >= 0)
            && IMPLICATION(alg == eltwise_clip, beta >= alpha)
            && IMPLICATION(alg == eltwise_round, dt == dnnl_f32)
            && IMPLICATION(one_of(dt, dnnl_s32, dnnl_s8, dnnl_u8),
                    one_of(alg, eltwise_relu, eltwise_linear));

    const bool eltwise_use_dst
            = one_of(alg, eltwise_relu_use_dst_for_bwd,
                      eltwise_tanh_use_dst_for_bwd, eltwise_elu_use_dst_for_bwd,
                      eltwise_sqrt_use_dst_for_bwd,
                      eltwise_logistic_use_dst_for_bwd,
                      eltwise_exp_use_dst_for_bwd)
            && IMPLICATION(one_of(alg, eltwise_relu_use_dst_for_bwd,
                                   eltwise_elu_use_dst_for_bwd),
                    alpha >= 0);

    return eltwise_use_src || eltwise_use_dst;
}

inline float get_bias(const char *bias, size_t offset, data_type_t data_type) {
    if (!bias) return 0.0f;

#define CASE(dt) \
    case dt: return (float)((const prec_traits<dt>::type *)bias)[offset]

    switch (data_type) {
        CASE(data_type::s8);
        CASE(data_type::u8);
        CASE(data_type::bf16);
        CASE(data_type::s32);
        CASE(data_type::f32);
        default: assert(!"unimplemented");
    }
    return 0; // never happens (should probably be a NaN)
#undef CASE
}

inline float get_sum(char *sum, size_t offset, data_type_t data_type)
{
    if (!sum)
        return 0.0f;

#define CASE(dt) \
    case dt: return (float)((const prec_traits<dt>::type *)sum)[offset]

    switch (data_type) {
        CASE(data_type::s8);
        CASE(data_type::u8);
        CASE(data_type::s32);
        CASE(data_type::f32);
        default: assert(!"unimplemented");
    }
    return 0; // never happens (should probably be a NaN)
#undef CASE
}

} // namespace math
} // namespace impl
} // namespace dnnl

#endif