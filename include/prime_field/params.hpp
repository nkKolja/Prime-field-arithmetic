#pragma once

#include "../common/types.hpp"
#include <array>

namespace prime_field {
namespace montgomery {

using namespace mp;


////////////////////////////////////////
/// MONTGOMERY PARAMETER COMPUTATION ///
////////////////////////////////////////

// Compute R mod p where R = 2^(N * RADIX)
template<size_t N>
constexpr std::array<digit_t, N> compute_Mont_one(const std::array<digit_t, N>& p) {
    std::array<digit_t, N + 1> mont_one_full = {}; mont_one_full[N] = 1;
    std::array<digit_t, N> mont_one_reduced = {};
    mp_div_r<N, N + 1>(mont_one_reduced, mont_one_full, p);

    return mont_one_reduced;
}

// Compute R^2 mod p
template<size_t N>
constexpr std::array<digit_t, N> compute_R2(const std::array<digit_t, N>& p) {
    std::array<digit_t, 2 * N + 1> mont_two_full = {}; mont_two_full[2 * N] = 1;
    std::array<digit_t, N> mont_two_reduced = {};
    mp_div_r<N, 2 * N + 1>(mont_two_reduced, mont_two_full, p);

    return mont_two_reduced;
}

// Compute R^3 mod p
template<size_t N>
constexpr std::array<digit_t, N> compute_R3(const std::array<digit_t, N>& p) {
    std::array<digit_t, 3 * N + 1> mont_three_full = {}; mont_three_full[3 * N] = 1;
    std::array<digit_t, N> mont_three_reduced = {};
    mp_div_r<N, 3 * N + 1>(mont_three_reduced, mont_three_full, p);

    return mont_three_reduced;
}

// Compute nip = -p^(-1) mod 2^(64 * N)
template<size_t N>
constexpr std::array<digit_t, N> compute_nip(const std::array<digit_t, N>& p) {
    // Invert
    std::array<digit_t, N> p_inv;
    mp_inv_mod<N, N>(p_inv, p);

    // Negate
    std::array<digit_t, N> zero = {};
    std::array<digit_t, N> out;
    mp_sub<N, N, N>(out, zero, p_inv);

    return out;
}

// Compute ip = p^(-1) mod 2^(64 * N)
template<size_t N>
constexpr std::array<digit_t, N> compute_ip(const std::array<digit_t, N>& p) {
    // Invert
    std::array<digit_t, N> out;
    mp_inv_mod<N, N>(out, p);
    return out;
}

// Compute iR = R^(-1) mod p where R = 2^(64 * N)
template<size_t N>
constexpr std::array<digit_t, N> compute_iR(const std::array<digit_t, N>& p) {
    
    // First compute 1/2 mod p
    std::array<digit_t, N + 1> iR = {}; iR[0] = 1;

    // N * RADIX many divisions by 2
    for (size_t i = 0; i < N * RADIX; i++) {
        if(iR[0] & 1) {
            mp_add<N + 1, N + 1, N>(iR, iR, p);
        }
        rshift<N + 1, N + 1>(iR, iR, 1);
    }
    std::array<digit_t, N> out = {};
    copy(out, iR);
    return out;
}


// Compute (p-1)/2
template<size_t N>
constexpr std::array<digit_t, N> compute_pm1_half(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> one = {};    one[0] = 1;
    std::array<digit_t, N> temp = {};
    std::array<digit_t, N> out = {};

    //Technically unnecessary to compute p-1 first but it makes it easier to read
    mp_sub<N, N, N>(temp, p, one);
    rshift<N, N>(out, temp, 1);
    return out;
}

// Compute (p+1)/2
template<size_t N>
constexpr std::array<digit_t, N> compute_pp1_half(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> one = {};    one[0] = 1;
    std::array<digit_t, N + 1> temp = {};
    std::array<digit_t, N> out = {};

    //Technically unnecessary to compute p-1 first but it makes it easier to read
    mp_add<N + 1, N, N>(temp, p, one);
    rshift<N, N + 1>(out, temp, 1);
    return out;
}

// Compute (p+1)//4
// Is useful when p ≡ 3 (mod 4)
template<size_t N>
constexpr std::array<digit_t, N> compute_pp1_quarter(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> one = {};    one[0] = 1;
    std::array<digit_t, N + 1> temp = {};
    std::array<digit_t, N> out = {};

    mp_add<N + 1, N, N>(temp, p, one);
    rshift<N, N + 1>(out, temp, 2);
    return out;
}


// Compute p-2
template<size_t N>
constexpr std::array<digit_t, N> compute_pm2(const std::array<digit_t, N>& p) {
    std::array<digit_t, N> two = {};    two[0] = 2;
    std::array<digit_t, N> out = {};

    mp_sub<N, N, N>(out, p, two);
    return out;
}



// Check if 2p overflows N words
template<size_t N>
constexpr bool compute_2p_overflow(const std::array<digit_t, N>& p) {
    return (bitsize(p) >= N * RADIX);
}

// Check if 3p overflows N words
template<size_t N>
constexpr bool compute_3p_overflow(const std::array<digit_t, N>& p) {
    std::array<digit_t, N + 1> temp;
    mp_add<N + 1, N, N>(temp, p, p);
    mp_add<N + 1, N + 1, N>(temp, temp, p);
    return (bitsize(temp) > N * RADIX);
}



// Compute floor(2^(2 * RADIX * N) / p)
template<size_t N, size_t AA>
constexpr std::array<digit_t, AA + 1> compute_barrett_mu(const std::array<digit_t, N>& p) {
    std::array<digit_t, N + AA + 1> RNA = {};    RNA[N + AA] = 1;
    std::array<digit_t, AA + 1> out = {};
    
    mp_div_q<AA + 1, N + AA + 1, N>(out, RNA, p);

    return out;
}



// Check the Barrett reduction quotent approximation factor
// If true the quotient approximation is at most off by one
// Otherwise it is at most off by two
template<size_t N, size_t AA>
constexpr int compute_barrett_appx_factor(const std::array<digit_t, N>& p) {
    std::array<digit_t, N + AA + 1> two_pow_na1 = {};
    std::array<digit_t, AA + 1> quo = {};
    std::array<digit_t, N> rem = {};
    two_pow_na1[N + AA] = 1;
    
    // 2^(RADIX * (N+AA)) = p * quo + rem
    mp_div_qr<AA + 1, N, N + AA + 1>(quo, rem, two_pow_na1, p);

    std::array<digit_t, N> two_pow_nm1 = {};
    std::array<digit_t, N + 1> res = {};
    two_pow_nm1[N - 1] = 1;

    mp_add(res, two_pow_nm1, rem);

    if (mp_compare<N + 1, N>(res, p) <= 0) {
        return 1;
    } else {
        return 2;
    }
}




// Define a Prime struct from just the prime array at compile-time
template<digit_t... PrimeDigits>
struct PrimeParameters {
    static constexpr size_t NWORDS = sizeof...(PrimeDigits);
    static constexpr std::array<digit_t, NWORDS> p = {PrimeDigits...};
    static constexpr size_t AWORDS = (RADIX == 64) ? 2 : 4; // How many words are needed to represent 128 bits
    
    // Static check: highest word must not be zero
    static_assert(p[NWORDS - 1] != 0, "Highest word of prime must not be zero");

    // Calculate NBITS = number of bits needed to represent p
    static constexpr size_t NBITS = bitsize(p);

    // Essential Montgomery parameters
    static constexpr std::array<digit_t, NWORDS> Mont_one = compute_Mont_one(p);
    static constexpr std::array<digit_t, NWORDS> R2 = compute_R2(p);
    static constexpr std::array<digit_t, NWORDS> R3 = compute_R3(p);
    static constexpr std::array<digit_t, NWORDS> nip = compute_nip(p);
    static constexpr std::array<digit_t, NWORDS> ip = compute_ip(p);
    static constexpr std::array<digit_t, NWORDS> iR = compute_iR(p);
    static constexpr std::array<digit_t, NWORDS> pm1_half = compute_pm1_half(p);
    static constexpr std::array<digit_t, NWORDS> pp1_half = compute_pp1_half(p);
    static constexpr std::array<digit_t, NWORDS> pp1_quarter = compute_pp1_quarter(p);
    static constexpr std::array<digit_t, NWORDS> pm2 = compute_pm2(p);

    // Essential Barrett parameters
    static constexpr bool px3_overflow = compute_3p_overflow(p);
    static constexpr bool px2_overflow = compute_2p_overflow(p);

};

// Helper macro to define a prime from just the digits
#define DEFINE_PRIME_FROM_ARRAY(Name, ...) \
    using Name = prime_field::montgomery::PrimeParameters<__VA_ARGS__>

} // namespace montgomery
} // namespace prime_field
