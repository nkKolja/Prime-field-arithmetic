#pragma once

#include "../common/types.hpp"
#include "../common/config.hpp"
#include "primitives.hpp"
#include <array>

namespace mp {

using namespace common;


/////////////////////////////////
// M U L T I P L I C A T I O N //
/////////////////////////////////

/* Multiplies two multiprecision numbers
------
Input:
out (K words)
in1 (N words)
in2 (M words)
-------
Output: 
out (K words) = (in1 * in2) mod K words  */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_mul(  std::array<digit_t, K>& out,
                                        const std::array<digit_t, N>& in1,
                                        const std::array<digit_t, M>& in2) {
    digit_t t = 0, u = 0, v = 0;
    digit_t hi = 0, lo = 0, carry = 0;

    for (size_t i = 0; i < K; i++) {
        size_t min_j = (i + 1 < M) ? 0 : (i - M + 1);
        size_t max_j = (i < N) ? i : N - 1;
        for (size_t j = min_j; j <= max_j; j++) {
            carry = 0;
            mul(hi, lo, in1[j], in2[i-j]);
            addc(v, carry, v, lo);
            addc(u, carry, u, hi);
            addc(t, carry, t, 0);
        }
        out[i] = v;
        v = u;
        u = t;
        t = 0;
    }
}


/* Multiplies two multiprecision numbers, divides the result by (1 << RADIX)
------
Input:
out (K words)
in1 (N words)
in2 (M words)
-------
Output: 
out (K words) = ((in1 * in2) >> (N + M - K) words) mod K words  */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_mul_high( 
    std::array<digit_t, K>& out,      
    const std::array<digit_t, N>& in1,                                        
    const std::array<digit_t, M>& in2) 
{
    static_assert(K <= N + M, "K must be less than or equal to N + M for mp_mul_high");
    digit_t t = 0, u = 0, v = 0;
    digit_t hi = 0, lo = 0, carry = 0;

    for (size_t i = 0; i < N + M; i++) {
        size_t min_j = (i + 1 < M) ? 0 : (i - M + 1);
        size_t max_j = (i < N) ? i : N - 1;
        for (size_t j = min_j; j <= max_j; j++) {
            carry = 0;
            mul(hi, lo, in1[j], in2[i-j]);
            addc(v, carry, v, lo);
            addc(u, carry, u, hi);
            addc(t, carry, t, 0);
        }
        if (i >= N + M - K)
            out[i - (N + M - K)] = v;
        v = u;
        u = t;
        t = 0;
    }
}


/* Multiples an N-word number with a single digit
------
Input:
out (N+1 words)
in (N words)
digit (1 word)
-------
Output: 
out (K words) = ((in * digit) >> P words) mod K words  */
template<size_t N>
MP_FORCE_INLINE constexpr void mp_mul_digit( 
    std::array<digit_t, N + 1>& out,
    const std::array<digit_t, N>& in,
    const digit_t& digit) 
{
    std::array<digit_t, N> temp{};
    digit_t carry = 0;

    for (size_t i = 0; i < N; i++)
        mul(temp[i], out[i], in[i], digit);

    for (size_t i = 1; i < N; i++)
        addc(out[i], carry, out[i], temp[i-1]);

    addc(out[N], carry, 0, temp[N-1]);
}


} // namespace mp
