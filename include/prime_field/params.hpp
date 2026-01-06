#pragma once

#include "types.hpp"
#include "detail/params_helpers.hpp"
#include <array>

namespace prime_field {
namespace montgomery {

////////////////////////////////////////
/// MONTGOMERY PARAMETER COMPUTATION ///
////////////////////////////////////////

// Compute R mod p where R = 2^(N * RADIX)
template<size_t N>
constexpr std::array<digit_t, N> compute_Mont_one(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> out = mod_pow_of_2<N>(p, N * RADIX);
    return out;
}

// Compute R^2 mod p
template<size_t N>
constexpr std::array<digit_t, N> compute_R2(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> out = mod_pow_of_2<N>(p, 2 * N * RADIX);
    return out;
}

// Compute pp = -p^(-1) mod 2^(64 * N)
template<size_t N>
constexpr std::array<digit_t, N> compute_pp(const std::array<digit_t, N>& p) {
    // Invert
    std::array<digit_t, N> p_inv = inv_mod_2_64_N(p);
    
    // Negate
    std::array<digit_t, N> zero = {};

    std::array<digit_t, N> out = sub<N, N, N>(zero, p_inv);
    return out;
}

// Compute ip = p^(-1) mod 2^(64 * N)
template<size_t N>
constexpr std::array<digit_t, N> compute_ip(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> out = inv_mod_2_64_N(p);
    return out;
}

// Compute 2^NBITS mod p
template<size_t N>
constexpr std::array<digit_t, N> compute_TWO_POW_NBITS(size_t NBITS, const std::array<digit_t, N>& p) {
    std::array<digit_t, N> out = mod_pow_of_2<N>(p, NBITS);
    return out;
}

// Compute (p-1)/2
template<size_t N>
constexpr std::array<digit_t, N> compute_pm1_half(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> one = {};
    one[0] = 1;
    
    std::array<digit_t, N> temp = sub<N, N, N>(p, one);
    std::array<digit_t, N> out = rshift<N, N>(temp, 1);
    return out;
}

// Compute (p+1)/2
template<size_t N>
constexpr std::array<digit_t, N> compute_pp1_half(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> one = {};
    one[0] = 1;
    
    std::array<digit_t, N + 1> temp = add<N + 1, N, N>(p, one);
    std::array<digit_t, N> out = rshift<N, N + 1>(temp, 1);
    return out;
}


// Compute p-2
template<size_t N>
constexpr std::array<digit_t, N> compute_pm2(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> two = {};
    two[0] = 2;
    
    std::array<digit_t, N> out = sub<N, N, N>(p, two);
    return out;
}


// Define a Prime struct from just the prime array at compile-time
template<digit_t... PrimeDigits>
struct PrimeParameters {
    static constexpr size_t NWORDS = sizeof...(PrimeDigits);
    static constexpr std::array<digit_t, NWORDS> p = {PrimeDigits...};
    
    // Static check: highest word must not be zero
    static_assert(p[NWORDS - 1] != 0, "Highest word of prime must not be zero");
    
    // Calculate NBITS = (NWORDS - 1) * RADIX + highest_bit_position(highest_word)
    static constexpr size_t NBITS = (NWORDS - 1) * RADIX + highest_bit_position(p[NWORDS - 1]);
    
    // Essential Montgomery parameters (only 4 needed)
    static constexpr std::array<digit_t, NWORDS> Mont_one = compute_Mont_one(p);
    static constexpr std::array<digit_t, NWORDS> R2 = compute_R2(p);
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = compute_TWO_POW_NBITS<NWORDS>(NBITS, p);
    static constexpr std::array<digit_t, NWORDS> pp = compute_pp(p);
    static constexpr std::array<digit_t, NWORDS> ip = compute_ip(p);
    static constexpr std::array<digit_t, NWORDS> iR = compute_iR(p);
    static constexpr std::array<digit_t, NWORDS> pm1_half = compute_pm1_half(p);
    static constexpr std::array<digit_t, NWORDS> pp1_half = compute_pp1_half(p);
    static constexpr std::array<digit_t, NWORDS> pm2 = compute_pm2(p);
};

// Helper macro to define a prime from just the digits
#define DEFINE_PRIME_FROM_ARRAY(Name, ...) \
    using Name = prime_field::montgomery::PrimeParameters<__VA_ARGS__>

} // namespace montgomery
} // namespace prime_field
