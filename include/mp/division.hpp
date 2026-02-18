#pragma once

#include "../common/types.hpp"
#include "../common/config.hpp"
#include "primitives.hpp"
#include "logic.hpp"
#include "multiplication.hpp"
#include <cassert>
#include <array>

namespace mp {

using namespace common;


/////////////////////
// D I V I S I O N //
/////////////////////

// DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS

/* Divides an N-word number by a single digit
------
Input:
quo (N words)
rem (1 word)
num (N words)
digit (1 word)
-------
Output: 
quo (N words) = (num / digit) mod N words
rem (1 word) = (num % digit) 
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t N>
MP_FORCE_INLINE constexpr void mp_div_digit_qr(    
    std::array<digit_t, N>& quo,
    digit_t& rem,
    const std::array<digit_t, N>& num,
    const digit_t& digit) 
{
    digit_t quo_digit;
    digit_t temp_hi = 0, temp_lo;
    for(int i = N; i-- >0; ) {
        temp_lo = num[i];
        div_qr(quo_digit, rem, temp_hi, temp_lo, digit);
        quo[i] = quo_digit;
        temp_hi = rem;
    }
}

/* Divides an N-word number by a single digit
------
Input:
quo (N words)
num (N words)
digit (1 word)
-------
Output: 
quo (N words) = (num / digit) mod N words
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t N>
MP_FORCE_INLINE constexpr void mp_div_digit_q(    
    std::array<digit_t, N>& quo,
    const std::array<digit_t, N>& num,
    const digit_t& digit) 
{
    digit_t rem_digit;
    mp_div_digit_qr(quo, rem_digit, num, digit);
}

/* Remainder of division of an N-word number by a single digit
------
Input:
rem (1 word)
num (N words)
digit (1 word)
-------
Output: 
rem (1 word) = (num % digit)
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t N>
MP_FORCE_INLINE constexpr void mp_div_digit_r(    
    digit_t& rem,
    const std::array<digit_t, N>& num,
    const digit_t& digit) 
{
    digit_t temp_hi = 0, temp_lo;
    for(int i = N; i-- >0; ) {
        temp_lo = num[i];
        div_r(rem, temp_hi, temp_lo, digit);
        temp_hi = rem;
    }
}






/* Divides an N-word number by a single digit with remainder, fast implementation
------
Input:
quo (N words)
rem (1 word)
num (N words)
digit (1 word)
-------
Output: 
quo (N words) = (num / digit) mod N words
rem (1 word) = (num % digit)
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t N>
MP_FORCE_INLINE constexpr void mp_fdiv_digit_qr(    
    std::array<digit_t, N>& quo,
    digit_t& rem,
    const std::array<digit_t, N>& num,
    const digit_t& digit) 
{
    assert(bitsize(digit) > 0);


    std::array<digit_t, N + 1> num_shifted = {};
    digit_t shift, apx;
    digit_t quo_digit;
    digit_t num_hi, num_lo;

    shift = RADIX - bitsize(digit);
    
    lshift<N+1, N>(num_shifted, num, shift);
    digit_t normalised_digit = digit << shift;
    apx_21(apx, normalised_digit);

    num_hi = num_shifted[N];
    num_shifted[N] = 0;

    for(int i = N; i-- >0;) {
        num_lo = num_shifted[i];
        fdiv_21_qr(quo_digit, rem, num_hi, num_lo, normalised_digit, apx);
        quo[i] = quo_digit;
        num_hi = rem;
    }

    rem >>= shift;
}





/* Divides an N-word number by a single digit, fast implementation
------
Input:
quo (N words)
num (N words)
digit (1 word)
-------
Output: 
quo (N words) = (num / digit) mod N words
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t N>
MP_FORCE_INLINE constexpr void mp_fdiv_digit_q(    
    std::array<digit_t, N>& quo,
    const std::array<digit_t, N>& num,
    const digit_t& digit) 
{
    assert(bitsize(digit) > 0);
    digit_t rem_digit;
    mp_fdiv_digit_qr(quo, rem_digit, num, digit);
}








/* Divides an N-word number by a single digit
------
Input:
quo (K words)
rem (N words)
num (M words)
den (N words)
-------
Output:
quo (K words) = (num / den) mod K words
rem (N words) = (num % den)
------
Notes:
Uses fast 3/2 division subroutine for quotient estimation
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_div_qr(    
    std::array<digit_t, K>& quo,
    std::array<digit_t, N>& rem,
    const std::array<digit_t, M>& num,
    const std::array<digit_t, N>& den)
{
    assert(bitsize(den[N-1]) > 0 && "Division by zero or highest word of divisor zero.");

    // If the divisor has more words than the dividend
    // the quotient is zero and the remainder is the dividend
    if constexpr(N > M) {
        copy(rem, num);
        zero(quo);
        return;
    }

    // Single digit division done exactly with primitive function
    if constexpr(N == 1) {
        std::array<digit_t, M> quotient;
        mp_fdiv_digit_qr<M>(quotient, rem[0], num, den[0]);
        copy(quo, quotient);
        return;
    }

    size_t shift = RADIX * N - bitsize(den);

    std::array<digit_t, M + 1> dividend = {};
    std::array<digit_t, N> divisor = {};
    std::array<digit_t, M - N + 1> quotient = {};
    digit_t d_hi, d_lo;
    digit_t apx;

    lshift<M + 1, M>(dividend, num, shift);
    lshift<N, N>(divisor, den, shift);
    d_hi = divisor[N-1];
    d_lo = divisor[N-2];
    apx_32(apx, d_hi, d_lo);

    for (int j = M - N; j >= 0; j--) {
        digit_t qhat;
        digit_t num_hi = dividend[j + N];
        digit_t num_mi = dividend[j + N - 1];
        digit_t num_lo = dividend[j + N - 2];

        digit_t borrow, carry;
        digit_t sink1, sink2;
        
        if (num_hi == d_hi && num_mi == d_lo) {
            qhat = MAX_DIGIT;
        } else {
            fdiv_32_qr(qhat, sink1, sink2, num_hi, num_mi, num_lo, d_hi, d_lo, apx);
        }

        // Multiply and subtract qhat * divisor from the dividend
        // qhat can still be off by one, this will be corrected in
        // case underflow occurs.
        std::array<digit_t, N + 1> temp_product;
        mp_mul_digit<N>(temp_product, divisor, qhat);

        quotient[j] = qhat;

        borrow = 0;
        for (size_t i = 0; i < N + 1; i++) {
            subc(dividend[j + i], borrow, dividend[j + i], temp_product[i]);
        }
        if (borrow) {
            // In case our estimate qhat is off by one
            quotient[j] = quotient[j] - 1;
            carry = 0;
            for (size_t i = 0; i < N; i++) {
                addc(dividend[j + i], carry, dividend[j + i], divisor[i]);
            }
        }
    }

    copy(quo, quotient);
    rshift<N, M + 1>(rem, dividend, shift);

}





/* Divides an N-word number by a single digit
------
Input:
quo (K words)
num (M words)
den (N words)
-------
Output:
quo (K words) = (num / den) mod K words
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t K, size_t M, size_t N>
MP_FORCE_INLINE constexpr void mp_div_q(    
    std::array<digit_t, K>& quo,
    const std::array<digit_t, M>& num,
    const std::array<digit_t, N>& den)
{
    std::array<digit_t, N> rem;
    mp_div_qr(quo, rem, num, den);
}

/* Divides an N-word number by a single digit
------
Input:
rem (N words)
num (M words)
den (N words)
-------
Output:
rem (N words) = (num % den)
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_div_r(    
    std::array<digit_t, N>& rem,
    const std::array<digit_t, M>& num,
    const std::array<digit_t, N>& den)
{
    if constexpr( N > M) {
        copy(rem, num);
        return;
    }
    std::array<digit_t, M - N + 1> quo;
    mp_div_qr(quo, rem, num, den);
}




/* Divides an N-word number by a single digit
------
Input:
quo (K words)
rem (N words)
num (M words)
den (N words)
-------
Output:
quo (K words) = (num / den) mod K words
rem (N words) = (num % den)
------
Notes:
Uses 2/1 fast division subroutine for quotient estimation
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_fdiv_21_qr(    
    std::array<digit_t, K>& quo,
    std::array<digit_t, N>& rem,
    const std::array<digit_t, M>& num,
    const std::array<digit_t, N>& den)
{
    assert(bitsize(den[N-1]) > 0 && "Division by zero or highest word of divisor zero.");

    // If the divisor has more words than the dividend
    // the quotient is zero and the remainder is the dividend
    if constexpr(N > M) {
        copy(rem, num);
        zero(quo);
        return;
    }

    // Single digit division done exactly with primitive function
    if constexpr(N == 1) {
        std::array<digit_t, M> quotient;
        mp_div_digit_qr<M>(quotient, rem[0], num, den[0]);
        copy(quo, quotient);
        return;
    }

    size_t shift = RADIX * N - bitsize(den);

    std::array<digit_t, M + 1> dividend = {};
    std::array<digit_t, N> divisor = {};
    std::array<digit_t, M - N + 1> quotient = {};
    digit_t apx;

    lshift<M + 1, M>(dividend, num, shift);
    lshift<N, N>(divisor, den, shift);
    apx_21(apx, divisor[N-1]);

    for (int j = M - N; j >= 0; j--) {
        digit_t qhat, rhat;
        digit_t num_hi = dividend[j + N];
        digit_t num_lo = dividend[j + N - 1];
        digit_t dhat = divisor[N - 1];

        digit_t temp_hi, temp_lo, borrow, carry;
        digit_t sink1, sink2;

        
        if (num_hi == dhat) {
            qhat = MAX_DIGIT;
            rhat = num_lo;
        } else {
            fdiv_21_qr(qhat, rhat, num_hi, num_lo, dhat, apx);
            mul(temp_hi, temp_lo, qhat, divisor[N - 2]);
            borrow = 0;
            // Check if qhat * v_{n-2} > rhat * b + u_{j+n-2}
            subc(sink1, borrow, dividend[j+N-2], temp_lo);
            subc(sink2, borrow, rhat, temp_hi);
            if (borrow) {
                qhat = qhat - 1;
                // rhat = rhat + d
                carry = 0;
                addc(rhat, carry, rhat, dhat);
                if (carry) {

                } else {
                    // This "if" is only an optimization to correct the 
                    // quotient qhat when it is off by two. This is a rare case
                    // And would nevertheless be corrected in the subtraction
                    // step belowwhen "u - qhat * v" is computed and checked
                    // for underflow.

                    // Update qhat * v_{n-2} for the new qhat
                    borrow = 0;
                    subc(temp_lo, borrow, temp_lo, divisor[N-2]);
                    subc(temp_hi, borrow, temp_hi, 0);

                    borrow = 0;
                    // Check if qhat * v_{n-2} > b * rhat + u_{j+n-2}
                    subc(temp_lo, borrow, dividend[j+N-2], temp_lo);
                    subc(temp_hi, borrow, rhat, temp_hi);
                    if (borrow) {
                        qhat = qhat - 1;
                        rhat = rhat + dhat;
                    }
                }
            }
        }
        

        // Multiply and subtract qhat * divisor from the dividend
        // qhat can still be off by one, this will be corrected in
        // case underflow occurs.
        std::array<digit_t, N + 1> temp_product;
        mp_mul_digit<N>(temp_product, divisor, qhat);

        quotient[j] = qhat;

        
        borrow = 0;
        for (size_t i = 0; i < N + 1; i++) {
            subc(dividend[j + i], borrow, dividend[j + i], temp_product[i]);
        }
        if (borrow) {
            // In case our estimate qhat is off by one
            quotient[j] = quotient[j] - 1;
            carry = 0;
            for (size_t i = 0; i < N; i++) {
                addc(dividend[j + i], carry, dividend[j + i], divisor[i]);
            }
        }
    }

    copy(quo, quotient);
    rshift<N, M + 1>(rem, dividend, shift);

}





/* Divides an N-word number by a single digit
------
Input:
quo (K words)
rem (N words)
num (M words)
den (N words)
-------
Output:
quo (K words) = (num / den) mod K words
rem (N words) = (num % den)
------
Notes:
Uses regular 2/1 division as subroutine for quotient estimation
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template<size_t K, size_t N, size_t M>
MP_FORCE_INLINE constexpr void mp_div_21_qr(    
    std::array<digit_t, K>& quo,
    std::array<digit_t, N>& rem,
    const std::array<digit_t, M>& num,
    const std::array<digit_t, N>& den)
{
    assert(bitsize(den[N-1]) > 0 && "Division by zero or highest word of divisor zero.");

    // If the divisor has more words than the dividend
    // the quotient is zero and the remainder is the dividend
    if constexpr(N > M) {
        copy(rem, num);
        zero(quo);
        return;
    }

    // Single digit division done exactly with primitive function
    if constexpr(N == 1) {
        std::array<digit_t, M> quotient;
        mp_div_digit_qr<M>(quotient, rem[0], num, den[0]);
        copy(quo, quotient);
        return;
    }

    size_t shift = RADIX * N - bitsize(den);

    std::array<digit_t, M + 1> dividend = {};
    std::array<digit_t, N> divisor = {};
    std::array<digit_t, M - N + 1> quotient = {};

    lshift<M + 1, M>(dividend, num, shift);
    lshift<N, N>(divisor, den, shift);

    for (int j = M - N; j >= 0; j--) {
        digit_t qhat, rhat;
        digit_t num_hi = dividend[j + N];
        digit_t num_lo = dividend[j + N - 1];
        digit_t dhat = divisor[N - 1];

        digit_t temp_hi, temp_lo, borrow, carry;
        digit_t sink1, sink2;

        
        if (num_hi == dhat) {
            qhat = MAX_DIGIT;
            rhat = num_lo;
        } else {
            div_qr(qhat, rhat, num_hi, num_lo, dhat);
            mul(temp_hi, temp_lo, qhat, divisor[N - 2]);
            borrow = 0;
            // Check if qhat * v_{n-2} > rhat * b + u_{j+n-2}
            subc(sink1, borrow, dividend[j+N-2], temp_lo);
            subc(sink2, borrow, rhat, temp_hi);
            if (borrow) {
                qhat = qhat - 1;
                // rhat = rhat + d
                carry = 0;
                addc(rhat, carry, rhat, dhat);
                if (carry) {

                } else {
                    // This "if" is only an optimization to correct the 
                    // quotient qhat when it is off by two. This is a rare case
                    // And would nevertheless be corrected in the subtraction
                    // step belowwhen "u - qhat * v" is computed and checked
                    // for underflow.

                    // Update qhat * v_{n-2} for the new qhat
                    borrow = 0;
                    subc(temp_lo, borrow, temp_lo, divisor[N-2]);
                    subc(temp_hi, borrow, temp_hi, 0);

                    borrow = 0;
                    // Check if qhat * v_{n-2} > b * rhat + u_{j+n-2}
                    subc(temp_lo, borrow, dividend[j+N-2], temp_lo);
                    subc(temp_hi, borrow, rhat, temp_hi);
                    if (borrow) {
                        qhat = qhat - 1;
                        rhat = rhat + dhat;
                    }
                }
            }
        }
        

        // Multiply and subtract qhat * divisor from the dividend
        // qhat can still be off by one, this will be corrected in
        // case underflow occurs.
        std::array<digit_t, N + 1> temp_product;
        mp_mul_digit<N>(temp_product, divisor, qhat);

        quotient[j] = qhat;

        
        borrow = 0;
        for (size_t i = 0; i < N + 1; i++) {
            subc(dividend[j + i], borrow, dividend[j + i], temp_product[i]);
        }
        if (borrow) {
            // In case our estimate qhat is off by one
            quotient[j] = quotient[j] - 1;
            carry = 0;
            for (size_t i = 0; i < N; i++) {
                addc(dividend[j + i], carry, dividend[j + i], divisor[i]);
            }
        }
    }

    copy(quo, quotient);
    rshift<N, M + 1>(rem, dividend, shift);

}





//////////////////////////////////////
// M O D U L A R  I N V E R S I O N //
//////////////////////////////////////


/* Inverts an N-word number modulo 2^K*RADIX, returns K words of the inverse
------
Input:
out (K words)
in (N words) (assumed 0 padded to K words if needed)
-------
Output:
out (K words) = (in^-1 mod 2^RADIX) mod K words
------
Notes:
DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS */
template <size_t K, size_t N>
MP_FORCE_INLINE constexpr void mp_inv_mod(
    std::array<digit_t, K>& out, 
    const std::array<digit_t, N>& in) 
{
    std::array<digit_t, K> x = {};
    std::array<digit_t, K> two = {};
    std::array<digit_t, K> ax;
    std::array<digit_t, K> two_minus_ax;

    two[0] = 2;
    // Initial inverse mod 2^RADIX
    inv_digit(x[0], in[0]);
    
    // Input already precise to RADIX bits, double precision each iteration
    for (size_t prec = 1; prec < N; prec *= 2) {
        mp_mul<K, K, N>(ax, x, in);
        mp_sub<K, K, K>(two_minus_ax, two, ax);
        mp_mul<K, K, K>(x, x, two_minus_ax);
    }
    copy(out, x);
}




















} // namespace mp
