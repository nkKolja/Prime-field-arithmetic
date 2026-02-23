#pragma once

#include "../common/types.hpp"
#include "../common/config.hpp"
#include "primitives.hpp"
#include <array>

namespace mp {

using namespace common;


///////////////////////////
// S U B T R A C T I O N //
///////////////////////////

/* Subtracts two multiprecision numbers, updates borrow
------
Input:
out (K words)
borrow (1 word)
in1 (N words) -> either truncated or zero-padded to K words
in2 (M words) -> either truncated or zero-padded to K words
-------
Output: 
out (K words) = (in1 - in2) mod K words
borrow (1 word) = final borrow value (only after the K'th word subtraction)
------
Notes:
Assumes borrow = 0 or 1 on input, updated to 0 or 1 on output
The borrow is only relative to the K'th word subtraction,
it might not be the same borrow from subtracting the full N + M words */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_subc( std::array<digit_t, K>& out,
                                        digit_t& borrow,
                                        const std::array<digit_t, N>& in1,
                                        const std::array<digit_t, M>& in2) {
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i < M) ? in2[i] : 0;
        subc(out[i], borrow, a, b);
    }
}
/* Subtracts a digit from a multiprecision number, updates borrow
------
Input:
out (K words)
borrow (1 word)
in1 (N words) -> either truncated or zero-padded to K words
in2 (digit)
-------
Output: 
out (K words) = (in1 - in2) mod K words
borrow (1 word) = final borrow value (only after the K'th word subtraction)
------
Notes:
Assumes borrow = 0 or 1 on input, updated to 0 or 1 on output
The borrow is only relative to the K'th word subtraction,
it might not be the same borrow from subtracting the full N + M words */
template<size_t K, size_t N>
MP_FORCE_INLINE constexpr void mp_subc( std::array<digit_t, K>& out,
                                        digit_t& borrow,
                                        const std::array<digit_t, N>& in1,
                                        digit_t in2) {
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i == 0) ? in2 : 0;
        subc(out[i], borrow, a, b);
    }
}

/* Subtracts two multiprecision numbers
------
Input:
out (K words)
in1 (N words) -> either truncated or zero-padded to K words
in2 (M words) -> either truncated or zero-padded to K words
-------
Output: 
out (K words) = (in1 - in2) mod K words */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_sub(  std::array<digit_t, K>& out,
                                        const std::array<digit_t, N>& in1,
                                        const std::array<digit_t, M>& in2) {
    digit_t borrow = 0;
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i < M) ? in2[i] : 0;
        subc(out[i], borrow, a, b);
    }
}

/* Subtracts a digit from a multiprecision number
------
Input:
out (K words)
in1 (N words) -> either truncated or zero-padded to K words
in2 (digit)
-------
Output:
out (K words) = (in1 - in2) mod K words */
template<size_t K, size_t N>
MP_FORCE_INLINE constexpr void mp_sub(  std::array<digit_t, K>& out,
                                        const std::array<digit_t, N>& in1,
                                        digit_t in2) {
    digit_t borrow = 0;
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i == 0) ? in2 : 0;
        subc(out[i], borrow, a, b);
    }
}


/* Subtracts two multiprecision numbers, adds the second number back if there is an underflow, 
------
Input:
out (K words)
in1 (N words)
in2 (M words)
-------
Output: 
out (K words) =  if (in1 >= in2) then { (in1 - in2) mod K words } else { in1 mod K words } */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_sub_conditional(  std::array<digit_t, K>& out,
                                                    const std::array<digit_t, N>& in1,
                                                    const std::array<digit_t, M>& in2) {
    digit_t borrow = 0;
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i < M) ? in2[i] : 0;
        subc(out[i], borrow, a, b);
    }
    if constexpr (N > K) {
        digit_t sink = 0;
        for (size_t i = K; i < N; i++) {
            digit_t b = (i < M) ? in2[i] : 0;
            subc(sink, borrow, in1[i], b);
        }
    }

    digit_t mask = 0 - borrow;
    digit_t carry = 0;
    for (size_t i = 0; i < K; i++){
        digit_t b = (i < M) ? in2[i] : 0;
        addc(out[i], carry, out[i], b & mask);
    }
}

/* Subtracts a digit from a multiprecision number, adds the digit back if there is an underflow, 
------
Input:
out (K words)
in1 (N words) -> either truncated or zero-padded to K words
in2 (digit) -> single digit to subtract
-------
Output: 
out (K words) =  if (in1 >= in2) mod K words { (in1 - in2) mod K words } else { in1 mod K words } */
template<size_t K, size_t N>
MP_FORCE_INLINE constexpr void mp_sub_conditional(  std::array<digit_t, K>& out,
                                                    const std::array<digit_t, N>& in1,
                                                    const digit_t& in2) {
    digit_t borrow = 0;
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i == 0) ? in2 : 0;
        subc(out[i], borrow, a, b);
    }
    if constexpr (N > K) {
        digit_t sink = 0;
        for (size_t i = K; i < N; i++) {
            digit_t b = (i == 0) ? in2 : 0;
            subc(sink, borrow, in1[i], b);
        }
    }
    
    digit_t mask = 0 - borrow;
    digit_t carry = 0;
    for (size_t i = 0; i < K; i++){
        digit_t b = (i == 0) ? in2 : 0;
        addc(out[i], carry, out[i], b & mask);
    }
}


} // namespace mp
