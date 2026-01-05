#pragma once

#include "types.hpp"
#include <array>

namespace prime_field {
namespace montgomery {

// Compile-time left shift by 1 bit with carry
template<size_t N>
constexpr void lshift1(std::array<digit_t, N>& a) {
    digit_t carry = 0;
    for (size_t i = 0; i < N; i++) {
        digit_t new_carry = a[i] >> (RADIX - 1);
        a[i] = (a[i] << 1) | carry;
        carry = new_carry;
    }
}

// Check if array is zero
template<size_t N>
constexpr bool mp_is_zero(const std::array<digit_t, N>& a) {
    for (size_t i = 0; i < N; i++) {
        if (a[i] != 0) return false;
    }
    return true;
}

// Check if array is one
template<size_t N>
constexpr bool mp_is_one(const std::array<digit_t, N>& a) {
    if (a[0] != 1) return false;
    for (size_t i = 1; i < N; i++) {
        if (a[i] != 0) return false;
    }
    return true;
}

// Compile-time comparison: returns 1 if a > p, 0 if a == p, -1 if a < p
template<size_t N>
constexpr int compare(const std::array<digit_t, N>& a, const std::array<digit_t, N>& p) {
    for (int i = N - 1; i >= 0; i--) {
        if (a[i] > p[i]) return 1;
        if (a[i] < p[i]) return -1;
    }
    return 0;
}

// Compile-time subtraction: a -= p
template<size_t N>
constexpr void sub(std::array<digit_t, N>& a, const std::array<digit_t, N>& p) {
    digit_t borrow = 0;
    for (size_t i = 0; i < N; i++) {
        digit_t old_a = a[i];
        a[i] = old_a - p[i] - borrow;
        borrow = (old_a < (p[i] + borrow)) ? 1 : 0;
    }
}

// Reduce once: if a >= p, then a -= p
template<size_t N>
constexpr void reduce_once(std::array<digit_t, N>& a, const std::array<digit_t, N>& p) {
    if (compare(a, p) >= 0) {
        sub(a, p);
    }
}

// Compute R mod p where R = 2^(N*64)
// Strategy: start with 1, double N*64 times, reducing after each double
template<size_t N>
constexpr std::array<digit_t, N> compute_Mont_one(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> R = {};
    R[0] = 1;
    
    for (size_t bit = 0; bit < N * RADIX; bit++) {
        lshift1(R);
        reduce_once(R, p);
    }
    
    return R;
}

// Compute R^2 mod p
// Strategy: start with R (Mont_one), double N*64 times, reducing after each double
template<size_t N>
constexpr std::array<digit_t, N> compute_R2(const std::array<digit_t, N>& p,
                                             const std::array<digit_t, N>& Mont_one) {
    std::array<digit_t, N> R2 = Mont_one;
    
    for (size_t bit = 0; bit < N * RADIX; bit++) {
        lshift1(R2);
        reduce_once(R2, p);
    }
    
    return R2;
}

// Compute 2^NBITS mod p
template<size_t N>
constexpr std::array<digit_t, N> compute_TWO_POW_NBITS(size_t NBITS, const std::array<digit_t, N>& p) {
    std::array<digit_t, N> result = {};
    result[0] = 1;
    
    for (size_t bit = 0; bit < NBITS; bit++) {
        lshift1(result);
        reduce_once(result, p);
    }
    
    return result;
}

// Compute -p^(-1) mod 2^64 using Newton iteration
constexpr digit_t compute_pp_word(digit_t p0) {
    digit_t x = p0;
    x = x * (2 - p0 * x);  // 2^6 bits
    x = x * (2 - p0 * x);  // 2^12 bits
    x = x * (2 - p0 * x);  // 2^24 bits
    x = x * (2 - p0 * x);  // 2^48 bits
    x = x * (2 - p0 * x);  // 2^96 bits
    return -x;
}

// Compute pp: only pp[0] is computed, rest filled with 0xFF...FF
template<size_t N>
constexpr std::array<digit_t, N> compute_pp(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> result;
    // Fill with 0xFF...FF for unused words
    for (size_t i = 0; i < N; i++) {
        result[i] = ~digit_t(0);
    }
    // Compute pp[0] = -p[0]^(-1) mod 2^64
    result[0] = compute_pp_word(p[0]);
    return result;
}

// Find the position of the highest set bit in a word (0-63)
constexpr size_t highest_bit_position(digit_t x) {
    if (x == 0) return 0;
    size_t pos = 0;
    if (x >= (digit_t(1) << 32)) { x >>= 32; pos += 32; }
    if (x >= (digit_t(1) << 16)) { x >>= 16; pos += 16; }
    if (x >= (digit_t(1) << 8))  { x >>= 8;  pos += 8; }
    if (x >= (digit_t(1) << 4))  { x >>= 4;  pos += 4; }
    if (x >= (digit_t(1) << 2))  { x >>= 2;  pos += 2; }
    if (x >= (digit_t(1) << 1))  { x >>= 1;  pos += 1; }
    if (x >= (digit_t(1) << 0))  {           pos += 1; }
    return pos;
}

// Define a Prime struct from just the prime array at compile-time
template<digit_t... PrimeDigits>
struct ComputedPrime {
    static constexpr size_t NWORDS = sizeof...(PrimeDigits);
    static constexpr std::array<digit_t, NWORDS> p = {PrimeDigits...};
    
    // Static check: highest word must not be zero
    static_assert(p[NWORDS - 1] != 0, "Highest word of prime must not be zero");
    
    // Calculate NBITS = (NWORDS - 1) * RADIX + highest_bit_position(highest_word)
    static constexpr size_t NBITS = (NWORDS - 1) * RADIX + highest_bit_position(p[NWORDS - 1]);
    
    // Essential Montgomery parameters (only 4 needed)
    static constexpr std::array<digit_t, NWORDS> Mont_one = compute_Mont_one(p);
    static constexpr std::array<digit_t, NWORDS> R2 = compute_R2(p, Mont_one);
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = compute_TWO_POW_NBITS<NWORDS>(NBITS, p);
    static constexpr std::array<digit_t, NWORDS> pp = compute_pp(p);
};

// Helper macro to define a prime from just the digits
#define DEFINE_PRIME_FROM_ARRAY(Name, ...) \
    using Name = prime_field::montgomery::ComputedPrime<__VA_ARGS__>

} // namespace montgomery
} // namespace prime_field
