#pragma once

#include <array>
#include <iostream>
#include <iomanip>
#include <cassert>
#include "../types.hpp"

namespace prime_field {


// Montgomery reduction
// Input: a in [0, R*p - 1], where R = 2^(RADIX * N)
// Output: aR^(-1) mod p in [0, p - 1]
template<typename Prime>
constexpr std::array<digit_t, Prime::NWORDS> montgomery_reduce(const std::array<digit_t, 2 * Prime::NWORDS>& in) {
    constexpr size_t N = Prime::NWORDS;

    std::array<digit_t, N> out = {};
    constexpr auto& p = Prime::p;
    constexpr auto& ip = Prime::ip;
    std::array<digit_t, N> temp_0 = {};
    std::array<digit_t, N> temp_1 = {};
    std::array<digit_t, 2 * N> temp_2 = {};
    digit_t mask, borrow, carry, waste;

    for (size_t i = 0; i < N; i++) temp_1[i] = in[i];

    temp_0 = mp_mul<N, N, N>(temp_1, ip);
    temp_2 = mp_mul<2 * N, N, N>(temp_0, p);

    borrow = 0;
    for (size_t i = 0; i < N; i++) {
        SUBC(waste, in[i], temp_2[i], borrow);
    }
    for (size_t i = 0; i < N; i++) {
        SUBC(out[i], in[N + i], temp_2[N + i], borrow);
    }

    // In case of underflow, add p back
    // Result will overflow and negate the underflow
    // so it will end in [0, p-1]
    mask = 0 - borrow;
    carry = 0;
    for (size_t i = 0; i < N; i++) {
        ADDC(out[i], out[i], p[i] & mask, carry);
    }

    return out;
}









} // namespace prime_field
