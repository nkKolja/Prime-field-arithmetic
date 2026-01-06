#pragma once

#include "types.hpp"
#include <array>

namespace prime_field {
namespace montgomery {



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

template<size_t N>
constexpr void rshift(std::array<digit_t, N>& a, size_t shift) {
    size_t words = shift / RADIX;
    size_t bits  = shift % RADIX;

    if (words >= N) {
        a.fill(0);
        return;
    }

    for (size_t i = 0; i < N - words; i++)
        a[i] = a[i + words];
    for (size_t i = N - words; i < N; i++)
        a[i] = 0;

    if (bits) {
        const size_t r = RADIX - bits;
        digit_t carry = 0;
        for (size_t i = N; i-- > 0;) {
            digit_t new_carry = a[i] << r;
            a[i] = (a[i] >> bits) | carry;
            carry = new_carry;
        }
    }
}

// Result is capped to N words, owerflow is discarded
template<size_t N>
constexpr void lshift1(std::array<digit_t, N>& a, size_t shift) {
    size_t words = shift / RADIX;
    size_t bits  = shift % RADIX;

    if (words >= N) {
        a.fill(0);
        return;
    }

    for (size_t i = N; i-- > words;)
        a[i] = a[i - words];
    for (size_t i = 0; i < words; i++)
        a[i] = 0;

    if (bits) {
        const size_t r = RADIX - bits;
        digit_t carry = 0;
        for (size_t i = 0; i < N; i++) {
            digit_t new_carry = a[i] >> r;
            a[i] = (a[i] << bits) | carry;
            carry = new_carry;
        }
    }
}

// Compile-time comparison: returns 1 if a > p, 0 if a == p, -1 if a < p
template<size_t N, size_t M>
constexpr int compare(const std::array<digit_t, N>& a, const std::array<digit_t, M>& b) {
    constexpr size_t min_size = (N <= M) ? N : M;
    
    if constexpr (N > M) {
        for (int i = N - 1; i >= static_cast<int>(M); i--) {
            if (a[i] != 0) return 1;
        }
    } else if constexpr (M > N) {
        for (int i = M - 1; i >= static_cast<int>(N); i--) {
            if (b[i] != 0) return -1;
        }
    }
    
    for (int i = min_size - 1; i >= 0; i--) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return -1;
    }
    return 0;
}

// Compile-time subtraction: a -= b
template<size_t N, size_t M>
constexpr void sub(std::array<digit_t, N>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    static_assert(M <= N, "Subtrahend should be smaller or equal in size then minuend");
    digit_t borrow = 0;
    for (size_t i = 0; i < N; i++) {
        digit_t old_in = in1[i];
        out[i] = old_in - in2[i] - borrow;
        borrow = (old_in < (in2[i] + borrow)) ? 1 : 0;
    }
    constexpr for (size_t i = N; i < M; i++) {
        digit_t old_in = in1[i];
        out[i] = old_in - 0 - borrow;
        borrow = (old_in < borrow) ? 1 : 0;
    }
}

// Reduce once: if a >= b, then a -= b
template<size_t N>
constexpr void reduce_once(std::array<digit_t, N>& a, const std::array<digit_t, N>& b) {
    if (compare(a, b) >= 0) {
        sub(a, b);
    }
}




// Compute R mod p where R = 2^(N * RADIX)
// Strategy: start with 1, double N * RADIX times, reducing after each double
// Extra word is used during computation in case of overflow
template<size_t N>
constexpr std::array<digit_t, N> mod_pow_of_2(const std::array<digit_t, N>& m, const size_t exp) {
    std::array<digit_t, N + 1> R = {};
    R[0] = 1;
    
    for (size_t bit = 0; bit < exp; bit++) {
        lshift(R, 1);
        reduce_once(R, m);
    }

    std::array<digit_t, N> result = {};
    for (size_t i = 0; i < N; i++) {
        result[i] = R[i];
    }
    return result;
}




// Constexpr N×N multiplication modulo 2 ^ (64 * RADIX)
// Result: low N words of a * b
template<size_t N>
constexpr std::array<digit_t, N> mul_low(const std::array<digit_t, N>& a, const std::array<digit_t, N>& b) {
    std::array<digit_t, N> result = {};
    
    for (size_t i = 0; i < N; i++) {
        uint128_t carry = 0;
        for (size_t j = 0; j < N - i; j++) {
            uint128_t prod = static_cast<uint128_t>(a[i]) * b[j] + result[i + j] + carry;
            result[i + j] = static_cast<digit_t>(prod);
            carry = prod >> 64;
        }
    }
    
    return result;
}

// Constexpr subtraction: result = 2 - a (mod 2^(N*64))
template<size_t N>
constexpr std::array<digit_t, N> two_minus(const std::array<digit_t, N>& a) {
    std::array<digit_t, N> result = {};
    result[0] = 2;
    
    digit_t borrow = 0;
    for (size_t i = 0; i < N; i++) {
        digit_t old_val = result[i];
        result[i] = old_val - a[i] - borrow;
        borrow = (old_val < (a[i] + borrow)) ? 1 : 0;
    }
    
    return result;
}

// Compute a^(-1) mod 2^64 using Hensel lifting
// in assumed to be odd
constexpr digit_t inv_mod_2_64(digit_t in) {
    static_assert(in & 1, "Input must be odd for modular inverse mod 2^64");
    digit_t x = in;         // 1 bit precision
    x = x * (2 - in * x);   // 2 bit precision
    x = x * (2 - in * x);   // 4 bit precision
    x = x * (2 - in * x);   // 8 bit precision
    x = x * (2 - in * x);   // 16 bit precision
    x = x * (2 - in * x);   // 32 bit precision
    x = x * (2 - in * x);   // 64 bit precision
    return x;
}

// Compute a^-1 mod 2^(N*64) using Hensel lifting
template <size_t N>
constexpr std::array<digit_t, N> inv_mod_2_64_N(const std::array<digit_t, N>& a) {
    std::array<digit_t, N> x = {};
    // Initial inverse mod 2^64
    x[0] = inv_mod_2_64(a[0]);
    
    // Input already precise to 64 bits, double precision each iteration
    for (size_t prec = 1; prec < N; prec *= 2) {
        std::array<digit_t, N> ax = mul_low(a, x);
        std::array<digit_t, N> two_minus_ax = two_minus(ax);
        x = mul_low(x, two_minus_ax);
    }
    
    return x;
}



////////////////////////////////////////
/// MONTGOMERY PARAMETER COMPUTATION ///
////////////////////////////////////////

// Compute R mod p where R = 2^(N * RADIX)
// Strategy: start with 1, double N * RADIX times, reducing after each double
// Extra word is used during computation in case of overflow
template<size_t N>
constexpr std::array<digit_t, N> compute_Mont_one(const std::array<digit_t, N>& p) {
    return mod_pow_of_2<N>(p, N * RADIX);
}

// Compute R^2 mod p
// Strategy: start with R (Mont_one), double N * RADIX times, reducing after each double
template<size_t N>
constexpr std::array<digit_t, N> compute_R2(const std::array<digit_t, N>& p) {
    return mod_pow_of_2<N>(p, 2 * N * RADIX);
}

// Compute pp = -p^(-1) mod 2^(64 * N)
template<size_t N>
constexpr std::array<digit_t, N> compute_pp(const std::array<digit_t, N>& p) {
    // Invert
    std::array<digit_t, N> p_inv = inv_mod_2_64_N(p);
    
    // Negate
    std::array<digit_t, N> result = {};
    std::array<digit_t, N> zero = {};

    sub(result, zero, p_inv);

    return result;
}

// Compute iR = R^(-1) mod p
template<size_t N>
constexpr std::array<digit_t, N> compute_iR(const std::array<digit_t, N>& p, const std::array<digit_t, N>& Mont_one) {
    // TODO: Implement R^(-1) mod p using GCD algorithm
    // For now, return placeholder as the value is not used
    std::array<digit_t, N> result = {};
    return result;
}

// Compute ip = p^(-1) mod 2^(64 * N)
template<size_t N>
constexpr std::array<digit_t, N> compute_ip(const std::array<digit_t, N>& p) {
    // Invert
    std::array<digit_t, N> result = inv_mod_2_64_N(p);

    return result;
}

// Compute 2^NBITS mod p
template<size_t N>
constexpr std::array<digit_t, N> compute_TWO_POW_NBITS(size_t NBITS, const std::array<digit_t, N>& p) {
    return mod_pow_of_2<N>(p, NBITS);
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
    static constexpr std::array<digit_t, NWORDS> R2 = compute_R2(p);
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = compute_TWO_POW_NBITS<NWORDS>(NBITS, p);
    static constexpr std::array<digit_t, NWORDS> pp = compute_pp(p);
    static constexpr std::array<digit_t, NWORDS> ip = compute_ip(p);
};

// Helper macro to define a prime from just the digits
#define DEFINE_PRIME_FROM_ARRAY(Name, ...) \
    using Name = prime_field::montgomery::ComputedPrime<__VA_ARGS__>

} // namespace montgomery
} // namespace prime_field
