#pragma once

#include <array>
#include <iostream>
#include <iomanip>
#include "types.hpp"

namespace prime_field {

// Basic arithmetic macros
#define MUL(hi, lo, in1, in2) {                                                         \
    uint128_t t0 = (uint128_t)(in1) * (uint128_t)(in2);                                 \
    (hi) = (uint64_t)(t0 >> 64);                                                        \
    (lo) = (uint64_t)t0;                                                                \
}

#define ADDC(out, in1, in2, carry) {                                                    \
    uint128_t t_addc = (uint128_t)(in1) + (uint128_t)(in2) + (uint128_t)(carry);        \
    (carry) = (uint64_t)(t_addc >> 64);                                                 \
    (out) = (uint64_t)t_addc;                                                           \
}

#define SUBC(out, in1, in2, borrow) {                                                   \
    uint128_t t_subc = (uint128_t)(in1) - (uint128_t)(in2) - (uint128_t)(borrow);       \
    (borrow) = (uint64_t)(t_subc >> (sizeof(uint128_t) * 8 - 1));                       \
    (out) = (uint64_t)t_subc;                                                           \
}

// Helper function to print arrays
template<size_t N>
void print_array(std::ostream& os, const std::array<digit_t, N>& arr) {
    os << "0x" << std::hex << std::setfill('0');
    for (size_t i = N; i-- > 0;) {
        os << std::setw(16) << arr[i];
    }
    os << std::dec;
}

// Stream output operator for arrays
template<size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<digit_t, N>& arr) {
    print_array(os, arr);
    return os;
}

// Reinterpret first N elements of array as std::array<digit_t, N>&
template<size_t N, size_t M>
std::array<digit_t, N>& array_truncate(std::array<digit_t, M>& arr) {
    static_assert(N <= M, "Cannot truncate to larger size");
    return *reinterpret_cast<std::array<digit_t, N>*>(arr.data());
}

/// MULTIPLICATION HELPERS ///

// Adds two N-word vectors
// Input: in1 (N words), in2 (M words)
// Output: out (N + 1 words)
template<size_t N, size_t M>
void mp_add(std::array<digit_t, N + 1>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    static_assert(M <= N, "Addend should be smaller or equal in size then sum");
    digit_t carry = 0;
    for (size_t i = 0; i < M; i++) {
        ADDC(out[i], in1[i], in2[i], carry);
    }
    for (size_t i = M; i < N; i++) {
        ADDC(out[i], in1[i], 0, carry);
    }
    out[N] = carry;
}

// Adds two N-word vectors
// Input: in1 (N words), in2 (M words)
// Output: out (N words) = in1 + in2
// Assumes input assumed that no overflow occurs
template<size_t N, size_t M>
void mp_add_no_overflow(std::array<digit_t, N>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    static_assert(M <= N, "Addend should be is smaller or equal in size then sum");
    digit_t carry = 0;
    for (size_t i = 0; i < M; i++) {
        ADDC(out[i], in1[i], in2[i], carry);
    }
    for (size_t i = M; i < N; i++) {
        ADDC(out[i], in1[i], 0, carry);
    }
}

// Adds two N-word vectors and divides by (1 << RADIX)
// Input: in1 (N words), in2 (N words)
// Output: out (N words) = (in1 + in2)  >> RADIX
// It assumes that the result is divisible by (1 << RADIX), and outputs the quotient
template<size_t N, size_t M>
void mp_add_and_divide(std::array<digit_t, N>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    static_assert(M <= N, "Addend should be is smaller or equal in size then sum");
    digit_t carry = 0;
    ADDC(out[0], in1[0], in2[0], carry);
    for (size_t i = 1; i < M; i++) {
        ADDC(out[i-1], in1[i], in2[i], carry);
    }
    for (size_t i = M; i < N; i++) {
        ADDC(out[i-1], in1[i], 0, carry);
    }
    out[N-1] = carry;
}


/// SUBTRACTION HELPERS ///

// Subtracts two N-word vectors
// Input: in1 (N words), in2 (N words)
// Output: out (N words) = in1 + in2
// Assumes in1 >= in2 so no borrow occurs
template<size_t N>
void mp_sub_no_borrow(std::array<digit_t, N>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2) {
    digit_t borrow = 0;
    for (size_t i = 0; i < N; i++) {
        SUBC(out[i], in1[i], in2[i], borrow);
    }
}

// Subtracts two N-word vectors and corrects in case of underflow
// Input: in1 (K words), in2 (M words)
// Output: out (N words) = if ( in1 >= in2 ) { in1 - in2 } else { in1 }
// Assumes output fits in N words
template<size_t N, size_t M>
void mp_sub_conditional_NM(std::array<digit_t, N>& out, const std::array<digit_t, M>& in1, const std::array<digit_t, N>& in2) {
    static_assert(N <= M, "Cannot subtract larger from smaller");
    digit_t mask, borrow, carry, temp;
    
    borrow = 0;
    for (size_t i = 0; i < N; i++) {
        SUBC(out[i], in1[i], in2[i], borrow);
    }
    for (size_t i = N; i < M; i++) {
        SUBC(temp, in1[i], 0, borrow);
    }

    mask = 0 - borrow;
    carry = 0;
    for (size_t i = 0; i < N; i++) {
        ADDC(out[i], out[i], in2[i] & mask, carry);
    }
}


// Subtracts two N-word vectors and corrects in case of underflow
// Input: in1 (K words), in2 (M words)
// Output: out (N words) = if ( in1 >= in2 ) { in1 - in2 } else { in1 }
template<size_t N>
void mp_sub_conditional(std::array<digit_t, N>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2) {
    digit_t mask, borrow, carry;
    
    borrow = 0;
    for (size_t i = 0; i < N; i++) {
        SUBC(out[i], in1[i], in2[i], borrow);
    }

    mask = 0 - borrow;
    carry = 0;
    for (size_t i = 0; i < N; i++) {
        ADDC(out[i], out[i], in2[i] & mask, carry);
    }
}

/// MULTIPLICATION HELPERS ///

// Multiples an N-word vector with a single digit
// Input: in (N words), digit
// Output: out (N+1 words)
template<size_t N>
void digit_mul(std::array<digit_t, N + 1>& out, const std::array<digit_t, N>& in, digit_t digit) {
    digit_t carry = 0;
    std::array<digit_t, N> temp{};

    for (size_t i = 0; i < N; i++) {
        MUL(temp[i], out[i], in[i], digit);
    }

    for (size_t i = 1; i < N; i++) {
        ADDC(out[i], out[i], temp[i-1], carry);
    }
    ADDC(out[N], 0, temp[N-1], carry);
}

// Schoolbook multiprecision multiplication
// Input: in1 (N words), in2 (M words)
// Output: out (N+M words) = in1 * in2
template<size_t N, size_t M>
void mp_mul(std::array<digit_t, N+M>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    digit_t t = 0, u = 0, v = 0;
    digit_t hi, lo, carry;

    constexpr size_t min_MN = (N < M) ? N : M;
    constexpr size_t max_MN = (N < M) ? M : N;
    
    // Select which array is which based on size
    const auto& array_min = (N < M) ? in1 : in2;
    const auto& array_max = (N < M) ? in2 : in1;
    
    for (size_t i = 0; i < min_MN; i++) {
        for (size_t j = 0; j <= i; j++) {
            MUL(hi, lo, array_min[j], array_max[i-j]);
            ADDC(v, v, lo, carry);
            ADDC(u, u, hi, carry);
            ADDC(t, t, 0, carry);
        }
        out[i] = v;
        v = u;
        u = t;
        t = 0;
    }
    
    for (size_t i = min_MN; i < max_MN; i++) {
        for (size_t j = 0; j < min_MN; j++) {
            MUL(hi, lo, array_min[j], array_max[i-j]);
            ADDC(v, v, lo, carry);
            ADDC(u, u, hi, carry);
            ADDC(t, t, 0, carry);
        }
        out[i] = v;
        v = u;
        u = t;
        t = 0;
    }
    
    for (size_t i = max_MN; i < min_MN + max_MN - 1; i++) {
        for (size_t j = i - max_MN + 1; j < min_MN; j++) {
            MUL(hi, lo, array_min[j], array_max[i-j]);
            ADDC(v, v, lo, carry);
            ADDC(u, u, hi, carry);
            ADDC(t, t, 0, carry);
        }
        out[i] = v;
        v = u;
        u = t;
        t = 0;
    }
    out[min_MN + max_MN - 1] = v;
}

} // namespace prime_field
