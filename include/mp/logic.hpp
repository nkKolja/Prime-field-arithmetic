#pragma once

#include "../common/types.hpp"
#include "../common/config.hpp"
#include <array>
#include <iosfwd>
#include <iomanip>

namespace mp {

using namespace common;

///////////////////////////////
// D I S P L A Y   T O O L S //
///////////////////////////////

/* Helper function to print arrays */
template<size_t N>
void print_array(std::ostream& os, const std::array<digit_t, N>& arr) {
    os << "0x" << std::hex << std::setfill('0');
    int hexsize = RADIX / 4;
    for (size_t i = N; i-- > 0;) {
        os << std::setw(hexsize) << arr[i];
    }
    os << std::dec;
}

/* Stream output operator for arrays */
template<size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<digit_t, N>& arr) {
    print_array(os, arr);
    return os;
}


//////////////////////////////////
// E Q U A L I T Y  C H E C K S //
//////////////////////////////////

/* Constant-time equality check for word: 1 if equal, 0 if not equal */
MP_FORCE_INLINE bool ct_equal(const digit_t in1, const digit_t in2) {
    volatile digit_t c = in1 ^ in2;
    return ((c | (0 - c)) >> (RADIX - 1)) ^ 1;
}

/* Constant-time equality check for array: 1 if equal, 0 if not equal */
template<size_t N>
MP_FORCE_INLINE bool ct_equal(const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2) {
    volatile digit_t c = 0;
    for (size_t i = 0; i < N; i++)
        c |= in1[i] ^ in2[i];
    return ((c | (0 - c)) >> (RADIX - 1)) ^ 1;
}

// Constant-time inequality check for word: 1 if not equal, 0 if equal
MP_FORCE_INLINE bool ct_nequal(const digit_t in1, const digit_t in2) {
    volatile digit_t c = in1 ^ in2;
    return ((c | (0 - c)) >> (RADIX - 1));
}

/* Constant-time inequality check for array: 1 if not equal, 0 if equal */
template<size_t N>
MP_FORCE_INLINE bool ct_nequal(const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2) {
    volatile digit_t c = 0;
    for (size_t i = 0; i < N; i++)
        c |= in1[i] ^ in2[i];
    return ((c | (0 - c)) >> (RADIX - 1));
}


/////////////////////////
// C O M P A R I S O N //
/////////////////////////

/* Comparison: returns 1 if in1 > in2, 0 if in1 == in2, -1 if in1 < in2
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t N, size_t M>
MP_FORCE_INLINE constexpr int mp_compare(const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2) {
    constexpr size_t max_size = (N > M) ? N : M;

    for (size_t i = max_size; i-- > 0; ) {
        const digit_t v1 = (i < N) ? in1[i] : 0;
        const digit_t v2 = (i < M) ? in2[i] : 0;

        if (v1 > v2) return 1;
        if (v1 < v2) return -1;
    }
    return 0;
}



/////////////
// C O P Y //
/////////////

/* Copy
------
Input:
out
in
-------
Output: 
out = in */
template<size_t N>
MP_FORCE_INLINE constexpr void copy(std::array<digit_t, N>& out, const std::array<digit_t, N>& in) {
    for (size_t i = 0; i < N; i++) {
        out[i] = in[i];
    }
}

/* Copy with truncation or zero padding
------
Input:
out
in
-------
Output: 
out = in truncated to K words or zero-padded to K words */
template<size_t K, size_t M>
MP_FORCE_INLINE constexpr void copy(std::array<digit_t, K>& out, const std::array<digit_t, M>& in) {
    for (size_t i = 0; i < K; i++) {
        out[i] = (i < M) ? in[i] : 0;
    }
}

/* Swap
------
Input:
in1
in2
-------
Output: 
in1 = in2
in2 = in1  */
MP_FORCE_INLINE constexpr void swap(digit_t& in1, digit_t& in2) {
    digit_t temp = in2;
    in2 = in1;
    in1 = temp;
}

/* Swap
------
Input:
in1
in2
-------
Output: 
in1 = in2
in2 = in1 
Padded with 0 and truncated accordingly */
template<size_t K, size_t M>
MP_FORCE_INLINE constexpr void swap(std::array<digit_t, K>& in1, std::array<digit_t, M>& in2) {
    for (size_t i = 0; i < K; i++) {
        digit_t temp = (i < M) ? in2[i] : 0;
        in2[i] = (i < K) ? in1[i] : 0;
        in1[i] = temp;
    }
    for (size_t i = K; i < M; i++) {
        in2[i] = 0;
    }
}

/* Copy with truncation or zero padding
------
Input:
out
-------
Output: 
out = {0} */
template<size_t K>
MP_FORCE_INLINE constexpr void zero(std::array<digit_t, K>& out) {
    for (size_t i = 0; i < K; i++) {
        out[i] = 0;
    }
}

/////////////////////////////
// C O N D I T I O N A L S //
/////////////////////////////

/* Conditional select: out = (cond) ? in1 : in2 for word */
MP_FORCE_INLINE void conditional_select(digit_t& out, const digit_t in1, const digit_t in2, bool cond) {
    volatile digit_t mask = static_cast<digit_t>(-(digit_t)cond);
    out = (in1 & mask) | (in2 & ~mask);
}

/* Conditional select: out = (cond) ? in1 : in2 for array
------
Input:
out
in1
in2
cond
-------
Output: 
out = if cond { in1 } else { in2 } */
template<size_t N>
MP_FORCE_INLINE void conditional_select(std::array<digit_t, N>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, N>& in2, const bool cond) {
    volatile digit_t mask = static_cast<digit_t>(-(digit_t)cond);

    for (size_t i = 0; i < N; i++) {
        out[i] = (in1[i] & mask) | (in2[i] & ~mask);
    }
}

/* Conditional select: out = (cond) ? in1 : in2 for array with truncation or zero padding
------
Input:
out
in1
in2
cond
-------
Output: 
out = if cond {in1 mod K words} else {in2 mod K words} padded with zeros */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE void conditional_select(std::array<digit_t, K>& out, const std::array<digit_t, N>& in1, const std::array<digit_t, M>& in2, const bool cond) {
    volatile digit_t mask = static_cast<digit_t>(-(digit_t)cond);

    for (size_t i = 0; i < K; i++) {
        volatile digit_t v1 = (i < N) ? in1[i] : 0;
        volatile digit_t v2 = (i < M) ? in2[i] : 0;
        out[i] = (v1 & mask) | (v2 & ~mask);
    }
}


/* Conditional select: swap if condition
------
Input:
in1
in2
cond
-------
Output: 
in1 = in2 if cond is true, otherwise in1 remains unchanged
in2 = in1 if cond is true, otherwise in2 remains unchanged */
MP_FORCE_INLINE void conditional_swap(digit_t& in1, digit_t& in2, bool cond) {
    volatile digit_t mask = static_cast<digit_t>(-(digit_t)cond);
    volatile digit_t temp = mask & (in1 ^ in2);
    in1 ^= temp;
    in2 ^= temp;
}

/* Conditional select: out = (cond) ? in1 : in2 for array
------
Input:
in1
in2
cond
-------
Output: 
in1 = in2 if cond is true, otherwise in1 remains unchanged
in2 = in1 if cond is true, otherwise in2 remains unchanged */
template<size_t N>
MP_FORCE_INLINE void conditional_swap(std::array<digit_t, N>& in1, std::array<digit_t, N>& in2, const bool cond) {
    digit_t mask = static_cast<digit_t>(-(digit_t)cond);

    for (size_t i = 0; i < N; i++) {
        volatile digit_t temp = mask & (in1[i] ^ in2[i]);
        in1[i] ^= temp;
        in2[i] ^= temp;
    }
}


/////////////
// S I Z E //
/////////////
// THESE FUNCTIONS ARE NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS

// // The bitsize of the word, e.g. number of bits needed to represent it
// // 0 -> 0, 1 -> 1, 2 -> 2, 3 -> 2, ..., MAX_DIGIT -> RADIX
// MP_FORCE_INLINE constexpr size_t bitsize(digit_t in) {
//     size_t out = 0;

//     // if (in >> 32) == 0; out += 32; in >>= 32;
//     // if (in >> 16) == 0; out += 16; in >>= 16;
//     // ...
//     for (size_t i = (RADIX >> 1); i > 0; i >>= 1) {
//         if (in >> i) {
//             in >>= i;
//             out += i;
//         }
//     }

//     return out;
// }


/* Helper function: count the number of significant bits in a digit
------
Returns the position of the most significant bit + 1, or 0 if digit is 0
Example: bitsize(0b1011) = 4, bitsize(0b1) = 1, bitsize(0) = 0 */
MP_FORCE_INLINE constexpr size_t bitsize(digit_t x) {
    if (x == 0) return 0;
#if defined(__GNUC__) || defined(__clang__)
    // Use compiler builtin for count leading zeros
    return RADIX - __builtin_clzll(x);
#else
    // Fallback: binary search for MSB
    size_t n = 0;
    if constexpr (RADIX == 64) && (x >= (1ULL << 32)) { x >>= 32; n += 32; }
    if (x >= (1ULL << 16)) { x >>= 16; n += 16; }
    if (x >= (1ULL << 8))  { x >>= 8;  n += 8; }
    if (x >= (1ULL << 4))  { x >>= 4;  n += 4; }
    if (x >= (1ULL << 2))  { x >>= 2;  n += 2; }
    if (x >= (1ULL << 1))  { x >>= 1;  n += 1; }
    if (x >= (1ULL << 0))  { n += 1; }
    return n;
#endif
}


// The bitsize of the array, e.g. number of bits needed to represent it
template<size_t N>
MP_FORCE_INLINE constexpr size_t bitsize(const std::array<digit_t, N>& in) {
    size_t out = 0;
    bool flag = false;

    for (size_t i = N; i-- > 0; ) {
        if (in[i] != 0) {
            return i * RADIX + bitsize(in[i]);
        }
    }

    return 0;
}

/////////////////////////////////////////
// C O N S T A N T - T I M E   S I Z E //
/////////////////////////////////////////

// The bitsize of the word, e.g. number of bits needed to represent it
// 0 -> 0, 1 -> 1, 2 -> 2, 3 -> 2, ..., MAX_DIGIT -> RADIX
MP_FORCE_INLINE constexpr size_t ct_bitsize(const digit_t in) {
    size_t out = 0;
    size_t in_copy = in;
    size_t mask, shift;

    // if (in >> 32) == 0; out += 32; in >>= 32;
    // if (in >> 16) == 0; out += 16; in >>= 16;
    // ...
    for (size_t i = (RADIX >> 1); i > 0; i >>= 1) {
        mask = -(digit_t)ct_nequal(in_copy >> i, 0);
        shift = mask & i;
        in_copy >>= shift;
        out += shift;
    }

    out += in_copy;

    return out;
}

// The bitsize of the array, e.g. number of bits needed to represent it
template<size_t N>
MP_FORCE_INLINE constexpr size_t ct_bitsize(const std::array<digit_t, N>& in) {
    size_t out = 0, flag = 0;
    digit_t temp, mask = -1;

    for (size_t i = N; i-- > 0; ) {
        conditional_select(temp, mask, in[i], flag);
        out += ct_bitsize(temp);
        flag |= ct_nequal(in[i], 0);
    }

    return out;
}


////////////////////////////////
// L O G I C A L  S H I F T S //
////////////////////////////////
// THESE FUNCTIONS ARE NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS

// Returns (in >> shift) truncated to K words
// NOT CONSTANT TIME IN THE SHIFT VALUE
template<size_t K, size_t N>
MP_FORCE_INLINE constexpr void rshift(std::array<digit_t, K>& out, const std::array<digit_t, N>& in, const size_t shift) {
    size_t words = shift / RADIX;
    size_t bits  = shift % RADIX;
    // TODO: add numeric limits check for i + words (+ 1) overflow
    for (size_t i = 0; i < K; i++) {
        digit_t lo = (i + words < N) ? in[i + words] >> bits : 0;
        digit_t hi = (i + words + 1 < N) && (bits != 0) ? in[i + words + 1] << (RADIX - bits) : 0;
        out[i] = hi | lo;
    }
}

// Returns (in << shift) truncated to K words
// NOT CONSTANT TIME IN THE SHIFT VALUE
template<size_t K, size_t N>
MP_FORCE_INLINE constexpr void lshift(std::array<digit_t, K>& out, const std::array<digit_t, N>& in, const size_t shift) {
    size_t words = shift / RADIX;
    size_t bits  = shift % RADIX;
    // TODO: add numeric limits check for i - words (- 1) underflow
    for (size_t i = 0; i < K; i++) {
        digit_t hi = (i >= words) && (i - words < N) ? in[i - words] << bits : 0;
        digit_t lo = (i >= words + 1) && (i - 1 - words < N) && (bits != 0) ? in[i - 1 - words] >> (RADIX - bits) : 0;
        out[i] = hi | lo;
    }
}



} // namespace mp
