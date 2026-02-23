#pragma once

#include "../common/types.hpp"
#include "../common/config.hpp"
#include "../mp/mp.hpp"
#include "params.hpp"
#include <array>

namespace mp {

using namespace common;

/* Montgomery reduction
------
Input:
out in [0, R*p - 1], where R = 2^(RADIX * N)
in (N words)
-------
Output:
out (K words) = aR^(-1) mod p in [0, p - 1] */
template<typename Prime>
constexpr void montgomery_reduce(std::array<digit_t, Prime::NWORDS> & out, const std::array<digit_t, 2 * Prime::NWORDS>& in) {
    constexpr size_t N = Prime::NWORDS;

    constexpr auto& p = Prime::p;
    constexpr auto& ip = Prime::ip;
    std::array<digit_t, N> temp_0 = {};
    std::array<digit_t, N> temp_1 = {};
    std::array<digit_t, 2 * N> temp_2 = {};
    digit_t mask, borrow, carry, waste;

    for (size_t i = 0; i < N; i++) temp_1[i] = in[i];

    mp_mul<N, N, N>(temp_0, temp_1, ip);
    mp_mul<2 * N, N, N>(temp_2, temp_0, p);

    borrow = 0;
    for (size_t i = 0; i < N; i++) {
        subc(waste, borrow, in[i], temp_2[i]);
    }
    for (size_t i = 0; i < N; i++) {
        subc(out[i], borrow, in[N + i], temp_2[N + i]);
    }

    // In case of underflow, add p back
    // Result will overflow and negate the underflow
    // so it will end in [0, p-1]
    mask = 0 - borrow;
    carry = 0;
    for (size_t i = 0; i < N; i++) {
        addc(out[i], carry, out[i], p[i] & mask);
    }
}




/* Barrett reduction
------
Input:
out (N words)  in [0, R*p - 1], where R = 2^(RADIX * N)
in (N + AWords words)
-------
Output:
out (N words) = (in mod p) in [0, p - 1] */
template<typename Prime, size_t A = Prime::NWORDS>
constexpr void barrett_reduce(std::array<digit_t, Prime::NWORDS>& out, const std::array<digit_t, Prime::NWORDS + A>& in) {
    constexpr size_t N = Prime::NWORDS;
    constexpr auto& p = Prime::p;
    constexpr auto barrett_mu = prime_field::montgomery::compute_barrett_mu<N, A>(p);
    constexpr auto barrett_appx_factor = prime_field::montgomery::compute_barrett_appx_factor<N, A>(p);
    constexpr auto X = N - 1;

    if constexpr (N == 0) {
        return;
    }


    std::array<digit_t, A + N - X> temp_0 = {};
    // std::array<digit_t, A + 1> temp_1 = {};              // barret_mu
    // std::array<digit_t, N + 2*A + 1 - X> temp_2 = {};    // t_0 * t_1 not needed
    std::array<digit_t, A + 1> temp_3 = {};
    // std::array<digit_t, N + A + 1> temp_4 = {};

    digit_t mask, borrow, carry;

    // Read highest A + 1 words of input (overflow bits)
    for (size_t i = 0; i < A + 1; i++)
        temp_0[i] = in[i + N - 1];


    mp_mul_high<A + 1, A + 1, A + 1>(temp_3, temp_0, barrett_mu);
    
    // We now compute temp_4 = qhat * p and later subtract it from the input
    // If barrett_appx_factor == 1 then q - 1 ≤ qhat ≤ q, so out < 2p after subtraction step. 
    // Therefore if 2p doesn't overflow then we can take only the lowest N words of the product since the rest will cancel out in the subtraction step.
    // Otherwise barrett_appx_factor == 2 and q - 2 ≤ qhat ≤ q, so out < 3p after subtraction step.
    // Therefore if 3p doesn't overflow then we can take only the lowest N words of the product since the rest will cancel out in the subtraction step.
    // This is checked at compile time and the optimal algorithm is chosen.
    if constexpr((barrett_appx_factor == 1 && !Prime::px2_overflow)) {
        // c - qhat p < 2p < 2^RADIX * N
        mp_mul<N, A + 1, N>(out, temp_3, p);
    
        // c = c - qhat p
        mp_sub<N, N + A, N>(out, in, out);

        // At most one correction step
        mp_sub_conditional<N, N, N>(out, out, p);

    } else if constexpr(barrett_appx_factor == 1 && Prime::px2_overflow) {
        std::array<digit_t, N + 1> temp_4 = {};

        // At most one correction step but original result cannt be guaranteed to fit in N words
        // c - qhat p < 2^RADIX * (N + 1)
        mp_mul<N + 1, A + 1, N>(temp_4, temp_3, p);

        // c = c - qhat p
        mp_sub<N + 1, N + A, N + 1>(temp_4, in, temp_4);

        // At most one correction step
        mp_sub_conditional<N + 1, N + 1, N>(temp_4, temp_4, p);
        copy(out, temp_4);

    } else if constexpr(barrett_appx_factor == 2 && !Prime::px3_overflow) {
        // c - qhat p < 3p < 2^RADIX * N
        mp_mul<N, A + 1, N>(out, temp_3, p);
    
        // c = c - qhat p
        mp_sub<N, N + A, N>(out, in, out);

        // At most two correction steps
        mp_sub_conditional<N, N, N>(out, out, p);
        mp_sub_conditional<N, N, N>(out, out, p);

    } else if constexpr(barrett_appx_factor == 2 && Prime::px3_overflow) {
        std::array<digit_t, N + 1> temp_4 = {};
        // At most two correction steps but original result cannt be guaranteed to fit in N words
        // c - qhat p < 2^RADIX * (N + 1)
        mp_mul<N + 1, A + 1, N>(temp_4, temp_3, p);

        // c = c - qhat p
        mp_sub<N + 1, N + A, N + 1>(temp_4, in, temp_4);

        // At most two correction steps
        mp_sub_conditional<N + 1, N + 1, N>(temp_4, temp_4, p);
        mp_sub_conditional<N + 1, N + 1, N>(temp_4, temp_4, p);
        copy(out, temp_4);
    }
}

} // namespace mp

