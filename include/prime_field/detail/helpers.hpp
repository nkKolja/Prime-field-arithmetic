#pragma once

#include <array>
#include <iostream>
#include <iomanip>
#include "../types.hpp"

namespace prime_field {


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
constexpr std::array<digit_t, N>& array_truncate(std::array<digit_t, M>& arr) {
    static_assert(N <= M, "Cannot truncate to larger size");
    return *reinterpret_cast<std::array<digit_t, N>*>(arr.data());
}


// Constant-time equality check for word - 1 if equal, 0 if not equal
constexpr bool ct_equal(const digit_t in1, const digit_t in2) {
    volatile digit_t c = in1 ^ in2;
    return ((c | (0 - c)) >> (RADIX - 1)) ^ 1;
}

// Constant-time equality check for array - 1 if equal, 0 if not equal
template<size_t N>
constexpr bool ct_equal(const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2) {
    volatile digit_t c = 0;
    for (size_t i = 0; i < N; i++)
        c |= in1[i] ^ in2[i];
    return ((c | (0 - c)) >> (RADIX - 1)) ^ 1;
}


// Constant-time inequality check for word - 1 if not equal, 0 if equal
constexpr bool ct_nequal(const digit_t in1, const digit_t in2) {
    volatile digit_t c = in1 ^ in2;
    return ((c | (0 - c)) >> (RADIX - 1));
}

// Constant-time inequality check for array - 1 if not equal, 0 if equal
template<size_t N>
constexpr bool ct_nequal(const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2) {
    volatile digit_t c = 0;
    for (size_t i = 0; i < N; i++)
        c |= in1[i] ^ in2[i];
    return ((c | (0 - c)) >> (RADIX - 1));
}



// Comparison: returns 1 if in1 > in2, 0 if in1 == in2, -1 if in1 < in2
// NOT CONSTANT TIME
template<size_t N, size_t M>
constexpr int compare(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    constexpr size_t max_size = (N > M) ? N : M;

    for (int i = max_size - 1; i >= 0; i--) {
        digit_t v1 = (i < N) ? in1[i] : 0;
        digit_t v2 = (i < M) ? in2[i] : 0;
        
        if (v1 > v2) return 1;
        if (v1 < v2) return -1;
    }

    return 0;
}


// Conditional select: c = (cond) ? b : a for word
constexpr digit_t conditional_select(const digit_t in1, const digit_t in2, bool cond) {
    digit_t out = {};
    digit_t mask = static_cast<digit_t>(-(digit_t)cond);
    
    out = (in1 & ~mask) | (in2 & mask);

    return out;
}

// Conditional select: c = (cond) ? b : a for array
template<size_t N>
constexpr std::array<digit_t, N> conditional_select(const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2, bool cond) {
    std::array<digit_t, N> out = {};
    digit_t mask = static_cast<digit_t>(-(digit_t)cond);
    
    for (size_t i = 0; i < N; i++) {
        out[i] = (in1[i] & ~mask) | (in2[i] & mask);
    }

    return out;
}


// Conditional swap: swaps a and b if cond != 0
constexpr void conditional_swap(digit_t& a, digit_t& b, bool cond) {
    digit_t mask = static_cast<digit_t>(-(digit_t)cond);
    digit_t temp = mask & (a ^ b);
    a ^= temp;
    b ^= temp;
}

// Conditional swap: swaps a and b if cond != 0
template<size_t N>
constexpr void conditional_swap(std::array<digit_t, N>& a, std::array<digit_t, N>& b, bool cond) {
    digit_t mask = static_cast<digit_t>(-(digit_t)cond);
    
    for (size_t i = 0; i < N; i++) {
        digit_t temp = mask & (a[i] ^ b[i]);
        a[i] ^= temp;
        b[i] ^= temp;
    }
}



// The bitsize of the word, e.g. number of bits needed to represent it
constexpr size_t bitsize(const digit_t in) {
    size_t pos = 0;
    size_t in_copy = in;
    size_t mask, shift;

    for (size_t i = 32; i > 0; i >>= 1) {
        mask = -(digit_t)ct_nequal(in_copy >> i, 0);
        shift = mask & i;
        in_copy >>= shift;
        pos += shift;
    }

    pos += in_copy;

    return pos;
}

// The bitsize of the array, e.g. number of bits needed to represent it
template<size_t N>
constexpr size_t bitsize(const std::array<digit_t, N>& in) {
    size_t out = 0, flag = 0;
    digit_t temp, mask = -1;

    for (int i = N - 1; i >= 0; i--) {
        temp = conditional_select(in[i], mask, flag);
        out += bitsize(temp);
        flag |= ct_nequal(in[i], 0);
    }

    return pos;
}




// Returns (in >> shift) truncated to K words
// NOT CONSTANT TIME IN THE SHIFT VALUE
template<size_t K, size_t N>
constexpr std::array<digit_t, K> rshift(const std::array<digit_t, N>& in, const size_t shift) {
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

// Returns (in << shift) truncated to K words
// NOT CONSTANT TIME IN THE SHIFT VALUE
template<size_t K, size_t N>
constexpr std::array<digit_t, K> lshift(std::array<digit_t, N>& in, const size_t shift) {
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




/// ADDITION HELPERS ///

// Adds two multiprecision numbers
// Input: in1 (N words), in2 (M words)
// Output: out (K words) = (in1 + in2) mod K words
// Overflow over K-sized array is discarded
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> mp_add(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    std::array<digit_t, K> out = {};
    digit_t carry = 0;

    size_t i = 0;

    for (; i < M && i < N && i < K; i++)
        ADDC(out[i], in1[i], in2[i], carry);

    for (; i < N && i < K; i++)
        ADDC(out[i], in1[i], 0, carry);

    for (; i < M && i < K; i++)
        ADDC(out[i], 0, in2[i], carry);

    for (; i < K; i++)
        ADDC(out[i], 0, 0, carry);

    return out;
}

// Adds two N-word vectors and divides by (1 << RADIX)
// Input: in1 (N words), in2 (N words)
// Output: out (N words) = (in1 + in2)  >> RADIX
// Overflow over K-sized array is discarded
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> mp_add_and_divide(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    std::array<digit_t, N> out = {};
    digit_t carry = 0;
    size_t i = 0;
    ADDC(out[0], in1[0], in2[0], carry); i++;

    for(; i < M && i < N && i < K; i++)
        ADDC(out[i-1], in1[i], in2[i], carry);

    for (; i < N && i < K; i++)
        ADDC(out[i-1], in1[i], 0, carry);

    for (; i < M && i < K; i++)
        ADDC(out[i-1], 0, in2[i], carry);

    for (; i < K; i++)
        ADDC(out[i-1], 0, 0, carry);

    return out;
}



/// SUBTRACTION HELPERS ///

// Subtracts two multiprecision numbers
// Input: in1 (N words), in2 (M words)
// Output: out (K words) = (in1 - in2) mod K words
// Underflow over K array is discarded
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> mp_sub(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    std::array<digit_t, K> out = {};
    digit_t borrow = 0;

    size_t i = 0;

    for (; i < N && i < M && i < K; i++)
        SUBC(out[i], in1[i], in2[i], borrow);

    for (; i < N && i < K; i++)
        SUBC(out[i], in1[i], 0, borrow);

    for (; i < M && i < K; i++)
        SUBC(out[i], 0, in2[i], borrow);

    for (; i < K; i++)
        SUBC(out[i], 0, 0, borrow);

    return out;
}

// Subtracts two N-word vectors and corrects in case of underflow
// Input: in1 (M words), in2 (N words)
// Output: out (K words) = if ( in1 >= in2 ) { in1 - in2 } else { in1 }
// Assumes output fits in K words
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> mp_sub_conditional(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    std::array<digit_t, N> out = {};
    digit_t temp, mask, borrow, carry;
    size_t i = 0, j = 0;

    for (; i < N && i < M && i < K; i++)
        SUBC(out[i], in1[i], in2[i], borrow);

    for (; i < N && i < K; i++)
        SUBC(out[i], in1[i], 0, borrow);

    for (; i < M && i < K; i++)
        SUBC(out[i], 0, in2[i], borrow);

    for (; i < K; i++)
        SUBC(out[i], 0, 0, borrow);

    mask = 0 - borrow;
    carry = 0;
    for (; j < K && j < N; j++)
        ADDC(out[j], out[j], in2[j] & mask, carry);

    for (; j < K; j++)
        ADDC(out[j], out[j], 0 & mask, carry);

}

// Reduce once: if a >= b, then return a - b, otherwise a
// Return truncated to K words
// NOT CONSTANT TIME
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> reduce_once(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    constexpr size_t min_nk = (N < K) ? N : K;
    std::array<digit_t, K> out = {};

    if (compare(in1, in2) >= 0) {
        out = sub<K, N, M>(in1, in2);
    }
    else {
        for (size_t i = 0; i < min_nk; i++) {
            out[i] = in1[i];
        }
    }

    return out;
}


/// MULTIPLICATION HELPERS ///

// Schoolbook multiprecision multiplication
// Input: in1 (N words), in2 (M words)
// Output: out (K words) = in1 * in2 mod K words
template<size_t K, size_t N, size_t M>
constexpr std::array<digit_t, K> mp_mul(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    std::array<digit_t, K> out = {};
    digit_t t = 0, u = 0, v = 0;
    digit_t hi, lo, carry;

    for (size_t i = 0; i < K; i++) {
        size_t min_j = (i + 1 < M) ? 0 : (i - M + 1);
        size_t max_j = (i < N) ? i : N - 1;
        for (size_t j = min_j; j <= max_j; j++) {
            carry = 0;
            MUL(hi, lo, in1[j], in2[i-j]);
            ADDC(v, v, lo, carry);
            ADDC(u, u, hi, carry);
            ADDC(t, t, 0, carry);
        }
        out[i] = v;
        v = u;
        u = t;
        t = 0;
    }
    return out;
}

// Schoolbook multiprecision multiplication
// High words only
// Input: in1 (N words), in2 (N words)
// Output: out (N words) = ((in1 * in2) >> P words) mod K words
template<size_t K, size_t N, size_t M, size_t P>
constexpr std::array<digit_t, K> mp_mul_high(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    std::array<digit_t, K> out = {};
    digit_t t = 0, u = 0, v = 0;
    digit_t hi, lo, carry;

    for (size_t i = 0; i < P + K; i++) {
        size_t min_j = (i + 1 < M) ? 0 : (i - M + 1);
        size_t max_j = (i < N) ? i : N - 1;
        for (size_t j = min_j; j <= max_j; j++) {
            carry = 0;
            MUL(hi, lo, in1[j], in2[i-j]);
            ADDC(v, v, lo, carry);
            ADDC(u, u, hi, carry);
            ADDC(t, t, 0, carry);
        }
        if (i >= P)
            out[i - P] = v;
        v = u;
        u = t;
        t = 0;
    }
    return out;
}

// Multiples an N-word vector with a single digit
// Input: in (N words), digit
// Output: out (N+1 words)
template<size_t N>
constexpr std::array<digit_t, N + 1> mp_mul_digit(const std::array<digit_t, N>& in, const digit_t digit) {
    std::array<digit_t, N + 1> out = {};
    std::array<digit_t, N> temp{};
    digit_t carry = 0;

    for (size_t i = 0; i < N; i++)
        MUL(temp[i], out[i], in[i], digit);

    for (size_t i = 1; i < N; i++)
        ADDC(out[i], out[i], temp[i-1], carry);

    ADDC(out[N], 0, temp[N-1], carry);
}


// Montgomery reduction
// Input: a in [0, R*p - 1], where R = 2^(RADIX * N)
// Output: aR^(-1) mod p in [0, p - 1]
template<size_t N>
constexpr std::array<digit_t, N> montgomery_reduce(const std::array<digit_t, 2 * N>& in) {
    static_assert(N == Prime::NWORDS, "montgomery_reduce: N must equal Prime::NWORDS");

    std::array<digit_t, N> out = {};
    constexpr auto& p = Prime::p;
    constexpr auto& ip = Prime::ip;
    std::array<digit_t, N> temp_0 = {};
    std::array<digit_t, N> temp_1 = {};
    std::array<digit_t, 2 * N> temp_2 = {};
    digit_t mask, borrow, carry, waste;

    for (size_t i = 0; i < N; i++) temp_1[i] = in[i];

    temp_0 = mp_mul(temp_1, ip);
    temp_2 = mp_mul(temp_0, p);

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
}




// Barrett reduction
// Input: a in [0, (R << A words) - 1], where R = 2^(RADIX * N)
// Output: a mod p in [0, p - 1]
template<size_t N, size_t A>
constexpr std::array<digit_t, N> barrett_reduce(const std::array<digit_t, N + A>& in) {
    static_assert(N == Prime::NWORDS, "barrett_reduce: N must equal Prime::NWORDS");

    std::array<digit_t, N> out = {};
    constexpr auto& p = Prime::p;
    constexpr auto& mu = Prime::barrett_mu;
    std::array<digit_t, A + 1> temp_0 = {};
    std::array<digit_t, N> temp_1 = {};
    std::array<digit_t, 2 * N> temp_2 = {};
    digit_t mask, borrow, carry, waste;

    for (size_t i = 0; i < A + 1; i++)
        temp_0[i] = in[i + N - 1];
    
    temp_0 = mp_mul(temp_1, ip);
    temp_2 = mp_mul(temp_0, p);

    borrow = 0;
    for (size_t i = 0; i < N; i++)
        SUBC(waste, in[i], temp_2[i], borrow);

    for (size_t i = 0; i < N; i++)
        SUBC(out[i], in[N + i], temp_2[N + i], borrow);

    // In case of underflow, add p back
    // Result will overflow and negate the underflow
    // so it will end in [0, p-1]
    mask = 0 - borrow;
    carry = 0;
    for (size_t i = 0; i < N; i++)
        ADDC(out[i], out[i], p[i] & mask, carry);

}



// Barrett reduction
// Input: a in [0, (1 << 2*N words) - 1]
// Output: a mod p in [0, p - 1]
template<size_t N>
constexpr std::array<digit_t, N> barrett_reduce(const std::array<digit_t, 2*N>& in) {
    static_assert(N == Prime::NWORDS, "barrett_reduce: N must equal Prime::NWORDS");

    std::array<digit_t, N> out = {};
    constexpr auto& p = Prime::p;
    constexpr auto& mu = Prime::barrett_mu;
    std::array<digit_t, N + 1> temp_0 = {};
    std::array<digit_t, N + 1> temp_2 = {};
    std::array<digit_t, 2*N + 1> temp_4 = {};
    digit_t mask, borrow, carry, waste;

    for (size_t i = 0; i < N + 1; i++)
        temp_0[i] = in[i + N - 1];
    
    temp_0 = mp_mul(temp_0, barrett_mu);
    temp_2 = mp_mul_high(temp_0, p);

    borrow = 0;
    for (size_t i = 0; i < N; i++)
        SUBC(waste, in[i], temp_2[i], borrow);

    for (size_t i = 0; i < N; i++)
        SUBC(out[i], in[N + i], temp_2[N + i], borrow);

    // In case of underflow, add p back
    // Result will overflow and negate the underflow
    // so it will end in [0, p-1]
    mask = 0 - borrow;
    carry = 0;
    for (size_t i = 0; i < N; i++)
        ADDC(out[i], out[i], p[i] & mask, carry);

}





// Compute result mod modulus where result = 2^exp
// Input : modulus (N words), exp
// Output: result = (2^exp) mod modulus (N words)
// Strategy: start with 1, double exp times, reducing after each double
template<size_t N>
constexpr std::array<digit_t, N> mod_pow_of_2(const std::array<digit_t, N>& modulus, const size_t exp) {
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





// Compute a^(-1) mod 2^RADIX using Hensel lifting
// Input assumed to be odd
// If even returns 0
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

// Compute a^(-1) mod 2^(RADIX * N) using Hensel lifting
// Input assumed to be odd
// If even returns 0
template <size_t N>
constexpr std::array<digit_t, N> inv_mod_2_64_N(const std::array<digit_t, N>& a) {
    std::array<digit_t, N> x = {};
    std::array<digit_t, N> two = {};
    two[0] = 2;
    // Initial inverse mod 2^RADIX
    x[0] = inv_mod_2_64(a[0]);
    
    // Input already precise to RADIX bits, double precision each iteration
    for (size_t prec = 1; prec < N; prec *= 2) {
        std::array<digit_t, N> ax = mp_mul<N, N, N>(a, x);
        std::array<digit_t, N> two_minus_ax = mp_sub<N, N, N>(two, ax);
        x = mp_mul<N, N, N>(x, two_minus_ax);
    }
    
    return x;
}



// Divides an N-word vector by a single digit
// Input: in (N words), digit
// Output: out (N words)
// NOT SAFE
template<size_t N>
constexpr std::array<digit_t, N> mp_div_digit(const std::array<digit_t, N>& in, const digit_t digit) {
    assert(digit != 0 && "Division by zero in mp_div_digit");
    std::array<digit_t, N> out = {};
    digit_t quo, rem;
    digit_t temp_hi = 0, temp_lo;

    for(int i = N - 1; i >= 0; i--) {
        temp_lo = in[i];
        DIV(quo, rem, temp_hi, temp_lo, digit);
        out[i] = quo;
        temp_hi = rem;
    }

    return out;
}


// Divides an N+1-word vector by an N-word vector
// Input: in1 (N+1 words), in2 (N words)
// Output: out (1 word)
// Assumes that highest bit of in2 is set to 1
// Assumes that in1[1:N] < in2[0:N-1]
// Assumes that N > 1 (otherwise use mp_div_digit)
// Knuth's Algorithm D p. 272 Chapter 4.3.1 AOCP Vol. 2
// Straightforward implementation - needs various optimizations
// NOT SAFE
template<size_t N>
constexpr digit_t approx_div_small_quotient(const std::array<digit_t, N + 1>& in1, const std::array<digit_t, N>& in2) {
    static_assert(N > 1 && "approx_div_small_quotient: N must be greater than 1.");
    assert(in2[N-1] >> (RADIX - 1) != 0 && "approx_div_small_quotient: Highest bit of divisor must be set.");
    assert(compare(rshift<N,N+1>(in1, RADIX), in2) < 0 && "approx_div_small_quotient: Numerator's N highest words must be smaller than denominator.");

    std::array<digit_t, 2> quo = {};
    std::array<digit_t, 2> rem = {};
    std::array<digit_t, 3> one = {};    one[0] = 1;
    std::array<digit_t, 3> zero = {};
    std::array<digit_t, 3> temp_0 = {};
    std::array<digit_t, 3> temp_1 = {};

    DIVR_DBL(quo[1], quo[0], rem[0], in1[N], in1[N-1], in2[N-1]);

    temp_0 = mp_mul_digit<3, 2, 1>(quo, in2[N-2]);
    temp_1[1] = rem[0];
    temp_1[0] = in1[N-2];

    for(size_t i = 0; i < 2 && (quo[1] > 0 || compare(temp_0, temp_1) > 0); i++){        
        
        quo = mp_sub<3, 3, 3>(quo, one);
        rem = mp_add<3, 2, 2>(rem, in2[N-1]);

        if(rem[1] != 0) {
            break;
        }
        temp_0 = mp_mul_digit<3, 2, 1>(quo, in2[N-2]);
        temp_1 = mp_add<3, 3, 2>(temp_1, rshift<2, 1>(in2[N-2], RADIX));
    }

    return quo[0];
}



// Divides an M-word vector by an N-word vector
// Input: in1 (M words), in2 (N words)
// Output: out (M - N + 1 words)
// Assumes M >= N
// Assumes highest word of in2 is non-zero
// Assumes 
// NOT SAFE
template<size_t M, size_t N>
constexpr std::array<digit_t, M - N + 1> mp_div(const std::array<digit_t, M>& in1, const std::array<digit_t, N>& in2) {
    static_assert(M >= N && "mp_div: M must be greater or equal to N");

    if constexpr(N == 1) {
        return mp_div_digit<M>(in1, in2[0]);
    }

    size_t shift = RADIX - bitsize(in2[N-1]);

    std::array<digit_t, M + 1> dividend = lshift<M + 1, M>(in1, shift);
    std::array<digit_t, N> divisor = lshift<N, N>(in2, shift);
    std::array<digit_t, M - N + 1> quotient = {};

    for (int j = M - N; j >= 0; j--) {
        std::array<digit_t, N + 1> current_dividend = {};
        for (size_t k = 0; k < N + 1; k++) {
            current_dividend[k] = dividend[j + k];
        }

        digit_t q_hat = approx_div_small_quotient<N>(current_dividend, divisor);
        quotient[j] = q_hat;

        std::array<digit_t, N + 1> product = mp_mul_digit<N + 1, N, 1>(divisor, q_hat);

        digit_t borrow = 0;
        for (size_t k = 0; k < N + 1; k++)
            SUBC(dividend[j + k], dividend[j + k], product[k], borrow);

            
        // If we had an underflow, add back the divisor
        digit_t mask = 0 - borrow;
        digit_t carry = 0;
        for (size_t k = 0; k < N; k++)
            ADDC(dividend[j + k], dividend[j + k], divisor[k] & mask, carry);

        ADDC(dividend[j + N], dividend[j + N], 0 & mask, carry);
    }


    quotient = rshift<M - N + 1, M - N + 1>(quotient, shift);
    
    return quotient;
}


} // namespace prime_field



// // Compile-time addition: returns in1 + in2
// // Overflow over K-sized array is discarded
// template<size_t K, size_t N, size_t M>
// constexpr void mp_add_inplace(std::array<digit_t, K>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
//     digit_t carry = 0;

//     size_t i = 0;

//     for (; i < M && i < N && i < K; i++)
//         ADDC(out[i], in1[i], in2[i], carry);

//     for (; i < N && i < K; i++)
//         ADDC(out[i], in1[i], 0, carry);

//     for (; i < M && i < K; i++)
//         ADDC(out[i], 0, in2[i], carry);

//     for (; i < K; i++)
//         ADDC(out[i], 0, 0, carry);
// }

// // Adds two N-word vectors and divides by (1 << RADIX)
// // Input: in1 (N words), in2 (N words)
// // Output: out (N words) = (in1 + in2)  >> RADIX
// // It assumes that the result is divisible by (1 << RADIX), and outputs the quotient
// template<size_t N, size_t M>
// void mp_add_and_divide_inplace(std::array<digit_t, N>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
//     static_assert(M <= N, "Addend should be is smaller or equal in size then sum");
//     digit_t carry = 0;
//     ADDC(out[0], in1[0], in2[0], carry);
//     for (size_t i = 1; i < M; i++)
//         ADDC(out[i-1], in1[i], in2[i], carry);

//     for (size_t i = M; i < N; i++)
//         ADDC(out[i-1], in1[i], 0, carry);

//     out[N-1] = carry;
// }
