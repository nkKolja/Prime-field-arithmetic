#pragma once

#include "../common/types.hpp"
#include "../common/config.hpp"
#include "primitives.hpp"
#include <array>

namespace mp {

using namespace common;


/////////////////////
// A D D I T I O N //
/////////////////////

/* Adds two multiprecision numbers, updates carry
------
Input:
out (K words)
carry (1 word)
in1 (N words) -> either truncated or zero-padded to K words
in2 (M words) -> either truncated or zero-padded to K words
-------
Output: 
out (K words) = (in1 + in2) mod K words
carry (1 word) = final carry value (only after the K'th word addition)
------
Notes:
Assumes carry = 0 or 1 on input, updated to 0 or 1 on output
The carry is only relative to the K'th word addition,
it might not be the same carry from adding the full N + M words */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_addc( std::array<digit_t, K>& out,
                                        digit_t& carry,
                                        const std::array<digit_t, N>& in1,
                                        const std::array<digit_t, M>& in2) {
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i < M) ? in2[i] : 0;
        addc(out[i], carry, a, b);
    }
}

/* Adds a digit to a multiprecision numbers, updates carry
------
Input:
out (K words)
carry (1 word)
in1 (N words) -> either truncated or zero-padded to K words
in2 (digit)
-------
Output: 
out (K words) = (in1 + in2) mod K words
carry (1 word) = final carry value (only after the K'th word addition)
------
Notes:
Assumes carry = 0 or 1 on input, updated to 0 or 1 on output
The carry is only relative to the K'th word addition,
it might not be the same carry from adding the full N + M words */
template<size_t K, size_t N>
MP_FORCE_INLINE constexpr void mp_addc( std::array<digit_t, K>& out,
                                        digit_t& carry,
                                        const std::array<digit_t, N>& in1,
                                        const digit_t& in2) {
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i == 0) ? in2 : 0;
        addc(out[i], carry, a, b);
    }
}



/* Adds two multiprecision numbers
------
Input:
out (K words)
in1 (N words) -> either truncated or zero-padded to K words
in2 (M words) -> either truncated or zero-padded to K words
-------
Output: 
out (K words) = (in1 + in2) mod K words */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_add(  std::array<digit_t, K>& out,
                                        const std::array<digit_t, N>& in1,
                                        const std::array<digit_t, M>& in2) {
    digit_t carry = 0;
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i < M) ? in2[i] : 0;
        addc(out[i], carry, a, b);
    }
}



/* Adds a digit to a multiprecision number
------
Input:
out (K words)
in1 (N words) -> either truncated or zero-padded to K words
in2 (digit)
-------
Output: 
out (K words) = (in1 + in2) mod K words */
template<size_t K, size_t N>
MP_FORCE_INLINE constexpr void mp_add(  std::array<digit_t, K>& out,
                                        const std::array<digit_t, N>& in1,
                                        const digit_t& in2) {
    digit_t carry = 0;
    for (size_t i = 0; i < K; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i == 0) ? in2 : 0;
        addc(out[i], carry, a, b);
    }
}


/* Adds two multiprecision numbers and divides the result by (1 << RADIX), updates carry
------
Input:
out (K words)
carry (1 word)
in1 (N words) -> either truncated or zero-padded to K + 1 words
in2 (M words) -> either truncated or zero-padded to K + 1 words
-------
Output: 
out (K words) = ((in1 + in2) >> RADIX) mod K words
carry (1 word) = final carry value (only after the K+1'th word addition)
------
Notes:
Assumes carry = 0 or 1 on input, updated to 0 or 1 on output
The carry is only relative to the K+1'th word addition,
it might not be the same carry from adding the full N + M words */
template<size_t K, size_t N, size_t M>
constexpr void mp_addc_and_divide(  std::array<digit_t, K>& out,
                                    digit_t& carry,
                                    const std::array<digit_t, N>& in1,
                                    const std::array<digit_t, M>& in2) {

    digit_t temp;

    // Add the first word of in1 and in2, along with the carry
    // This is only to check if there will be a carry propagation
    // The value stored in temp is discarded.
    if constexpr (K > 0) {
        digit_t a = (0 < N) ? in1[0] : 0;
        digit_t b = (0 < M) ? in2[0] : 0;
        addc(temp, carry, a, b);
    }

    // Write the rest of the words, shifting right by one word (i.e. dividing by RADIX)
    for (size_t i = 1; i < K + 1; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i < M) ? in2[i] : 0;
        addc(out[i-1], carry, a, b);
    }
}

/* Adds two multiprecision numbers and divides the result by (1 << RADIX)
------
Input:
out (K words)
in1 (N words) -> either truncated or zero-padded to K + 1 words
in2 (M words) -> either truncated or zero-padded to K + 1 words
-------
Output: 
out (K words) = ((in1 + in2) >> RADIX) mod K words */
template<size_t K, size_t N, size_t M>
constexpr void mp_add_and_divide(   std::array<digit_t, K>& out,
                                    const std::array<digit_t, N>& in1,
                                    const std::array<digit_t, M>& in2) {
    digit_t sink;
    digit_t carry = 0;
    // Add the first word of in1 and in2
    // This is only to check if there will be a carry propagation
    // The value stored in sink is discarded.
    if constexpr (K > 0) {
        digit_t a = (0 < N) ? in1[0] : 0;
        digit_t b = (0 < M) ? in2[0] : 0;
        addc(sink, carry, a, b);
    }

    // Write the rest of the words, shifting right by one word (i.e. dividing by RADIX)
    for (size_t i = 1; i < K + 1; i++){
        digit_t a = (i < N) ? in1[i] : 0;
        digit_t b = (i < M) ? in2[i] : 0;
        addc(out[i-1], carry, a, b);
    }
}


} // namespace mp
