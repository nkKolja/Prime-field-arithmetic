#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {
namespace montgomery {

// Find the position of the highest set bit in a word (0-63)
constexpr size_t highest_bit_position(digit_t in) {
    size_t pos = 0;
    if (in >= (digit_t(1) << 32)) { in >>= 32; pos += 32; }
    if (in >= (digit_t(1) << 16)) { in >>= 16; pos += 16; }
    if (in >= (digit_t(1) << 8))  { in >>= 8;  pos += 8; }
    if (in >= (digit_t(1) << 4))  { in >>= 4;  pos += 4; }
    if (in >= (digit_t(1) << 2))  { in >>= 2;  pos += 2; }
    if (in >= (digit_t(1) << 1))  { in >>= 1;  pos += 1; }
    if (in >= (digit_t(1) << 0))  {            pos += 1; }
    return pos;
}

// Returns (a >> shift) truncated to K words
template<size_t K, size_t N>
constexpr std::array<digit_t, K> rshift(const std::array<digit_t, N>& in, size_t shift) {
    size_t words = shift / RADIX;
    size_t bits  = shift % RADIX;

    std::array<digit_t, K> out = {};

    int end = (N - words < K) ? N - words : K;
    for (int i = 0; i < end; i++){
        out[i] = (in[i + words] >> bits);
        if (bits > 0 && (i + words + 1) < N) {
            out[i] |= (in[i + words + 1] << (RADIX - bits));
        }
    }

    return out;
}

// Returns (a << shift) truncated to K words
template<size_t K, size_t N>
constexpr std::array<digit_t, K> lshift(std::array<digit_t, N>& in, size_t shift) {
    size_t words = shift / RADIX;
    size_t bits  = shift % RADIX;

    std::array<digit_t, K> out = {};

    int end = (N < K - words) ? N : K - words;
    for (int i = 0; i < end; i++){
        out[i + words] = (in[i] << bits);
        if (bits > 0 && i > 0) {
            out[i + words] |= (in[i - 1] >> (RADIX - bits));
        }
    }

    return out;
}

// Comparison: returns 1 if in1 > in2, 0 if in1 == in2, -1 if in1 < in2
template<size_t N, size_t M>
constexpr int compare(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    constexpr size_t min_size = (N <= M) ? N : M;
    
    if constexpr (N > M) {
        for (int i = N - 1; i >= static_cast<int>(M); i--) {
            if (in1[i] != 0) return 1;
        }
    } else if constexpr (M > N) {
        for (int i = M - 1; i >= static_cast<int>(N); i--) {
            if (in2[i] != 0) return -1;
        }
    }
    
    for (int i = min_size - 1; i >= 0; i--) {
        if (in1[i] > in2[i]) return 1;
        if (in1[i] < in2[i]) return -1;
    }
    return 0;
}

// Compile-time addition: returns in1 + in2
// Overflow over K array is discarded
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> add(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    static_assert(K >= N && K >= M, "Sum size must be at least as large as both addends");
    
    std::array<digit_t, K> out{};
    digit_t carry = 0;
    
    constexpr size_t min_size = (N < M) ? N : M;
    for (size_t i = 0; i < min_size; i++) {
        const digit_t sum = in1[i] + carry;
        out[i] = sum + in2[i];

        carry = (sum < carry) || (out[i] < sum) ? 1 : 0;
    }
    
    for (size_t i = min_size; i < N; i++) {
        const digit_t sum = in1[i] + carry;
        out[i] = sum;
        carry = (sum < carry) ? 1 : 0;
    }
    for (size_t i = min_size; i < M; i++) {
        const digit_t sum = in2[i] + carry;
        out[i] = sum;
        carry = (sum < carry) ? 1 : 0;
    }
    
    for (size_t i = std::max(N, M); i < K; i++) {
        const digit_t sum = out[i] + carry;
        out[i] = sum;
        carry = (sum < carry) ? 1 : 0;
    }
    
    return out;
}


// Compile-time subtraction: returns in1 - in2
// Underflow over K array is discarded
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> sub(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    static_assert(M <= N, "Minunend size must be at least as large as subtrahend");
    static_assert(N <= K, "Difference size must be at least as large as minuend");

    std::array<digit_t, K> out{};
    digit_t borrow = 0;

    for (size_t i = 0; i < M; i++) {
        const digit_t diff1 = in1[i] - borrow;
        const digit_t diff2 = diff1 - in2[i];
        out[i] = diff2;
        borrow = (diff1 > in1[i]) || (diff2 > diff1) ? 1 : 0;
    }

    for (size_t i = M; i < N; i++) {
        const digit_t diff = in1[i] - borrow;
        out[i] = diff;
        borrow = (diff > in1[i]) ? 1 : 0;
    }

    for (size_t i = N; i < K; i++) {
        const digit_t diff = 0 - borrow;
        out[i] = diff;
        borrow = (diff > 0) ? 1 : 0;
    }

    return out;
}


// Reduce once: if a >= b, then a -= b
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> reduce_once(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    static_assert(N <= K, "Difference size must be at least as large as minuend");
    std::array<digit_t, K> out = {};

    if (compare(in1, in2) >= 0) {
        out = sub<K, N, M>(in1, in2);
    }
    else {
        for (size_t i = 0; i < N; i++) {
            out[i] = in1[i];
        }
    }

    return out;
}

// Compute result mod modulus where result = 2^exp
// Strategy: start with 1, double exp times, reducing after each double
template<size_t N>
constexpr std::array<digit_t, N> mod_pow_of_2(const std::array<digit_t, N>& modulus, size_t exp) {
    std::array<digit_t, N + 1> R = {};
    R[0] = 1;
    
    for (size_t bit = 0; bit < exp; bit++) {
        R = lshift<N + 1, N + 1>(R, 1);
        R = reduce_once<N + 1, N + 1, N>(R, modulus);
    }

    std::array<digit_t, N> result = {};
    for (size_t i = 0; i < N; i++) {
        result[i] = R[i];
    }
    return result;
}

// Constexpr N×N multiplication modulo 2^(N*64)
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
// Input assumed to be odd
constexpr digit_t inv_mod_2_64(digit_t in) {
    digit_t x = in;         // 1 bit precision
    x = x * (2 - in * x);   // 2 bit precision
    x = x * (2 - in * x);   // 4 bit precision
    x = x * (2 - in * x);   // 8 bit precision
    x = x * (2 - in * x);   // 16 bit precision
    x = x * (2 - in * x);   // 32 bit precision
    x = x * (2 - in * x);   // 64 bit precision
    return x;
}

// Compute a^(-1) mod 2^(N*64) using Hensel lifting
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

} // namespace montgomery
} // namespace prime_field
