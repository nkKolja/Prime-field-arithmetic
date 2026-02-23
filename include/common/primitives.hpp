#pragma once

#include "types.hpp"
#include "config.hpp"

namespace common {

/////////////////////
// A D D I T I O N //
/////////////////////

/* Adds two digit_t numbers, updates carry
------
Input:
out
carry
in1
in2
-------
Output: 
out = (in1 + in2 + carry) mod 2^RADIX
carry = updated carry value
------
Notes:
Assumes carry = 0 or 1 on input, updated to 0 or 1 on output */
MP_FORCE_INLINE constexpr void addc(
    digit_t& out, 
    digit_t& carry, 
    const digit_t in1, 
    const digit_t in2) 
{
    digit2_t t = static_cast<digit2_t>(in1) + static_cast<digit2_t>(in2) + static_cast<digit2_t>(carry);
    out = static_cast<digit_t>(t);
    carry = static_cast<digit_t>(t >> RADIX);
}

/* Adds two digit_t numbers, discards overflow
------
Input:
out
in1
in2
-------
Output: 
out = (in1 + in2) mod 2^RADIX */
MP_FORCE_INLINE constexpr void add(
    digit_t& out, 
    const digit_t in1, 
    const digit_t in2) 
{
    out = in1 + in2;
}




///////////////////////////
// S U B T R A C T I O N //
///////////////////////////

/* Subtracts two digit_t numbers, updates borrow
------
Input:
out
borrow
in1
in2
-------
Output: 
out = (in1 - in2 - borrow) mod 2^RADIX
borrow = updated borrow value
------
Notes:
Assumes borrow = 0 or 1 on input, updated to 0 or 1 on output */
MP_FORCE_INLINE constexpr void subc(
    digit_t& out, 
    digit_t& borrow, 
    const digit_t in1, 
    const digit_t in2) 
{
    digit2_t t = static_cast<digit2_t>(in1) - static_cast<digit2_t>(in2) - static_cast<digit2_t>(borrow);
    out = static_cast<digit_t>(t);
    borrow = static_cast<digit_t>(t >> RADIX) & 1;
}


/* Subtracts two digit_t numbers, discards underflow
------
Input:
out
in1
in2
-------
Output: 
out = (in1 - in2) mod 2^RADIX */
MP_FORCE_INLINE constexpr void sub(
    digit_t& out, 
    const digit_t in1, 
    const digit_t in2) 
{
    out = in1 - in2;
}



/////////////////////////////////
// M U L T I P L I C A T I O N //
/////////////////////////////////

/* Multiplies two digit_t numbers
------
Input:
out_hi
out_lo
in1
in2
-------
Output: 
out_hi = (in1 * in2) >> RADIX
out_lo = (in1 * in2) mod 2^RADIX */
MP_FORCE_INLINE constexpr void mul(
    digit_t& out_hi, 
    digit_t& out_lo, 
    const digit_t in1, 
    const digit_t in2) 
{
    digit2_t t = static_cast<digit2_t>(in1) * static_cast<digit2_t>(in2);
    out_lo = static_cast<digit_t>(t);
    out_hi = static_cast<digit_t>(t >> RADIX);
}

/* Multiplies two digit_t numbers, takes the high part of the result
------
Input:
out
in1
in2
-------
Output: 
out = (in1 * in2) >> 2^RADIX */
MP_FORCE_INLINE constexpr void mul_hi(
    digit_t& out, 
    const digit_t in1, 
    const digit_t in2) 
{
    digit2_t t = static_cast<digit2_t>(in1) * static_cast<digit2_t>(in2);
    out = static_cast<digit_t>(t >> RADIX);
}

/* Multiplies two digit_t numbers, takes the low part of the result
------
Input:
out
in1
in2
-------
Output: 
out = (in1 * in2) mod 2^RADIX */
MP_FORCE_INLINE constexpr void mul_lo(
    digit_t& out, 
    const digit_t in1, 
    const digit_t in2) 
{
    out = in1 * in2;
}


/////////////////////
// D I V I S I O N //
/////////////////////

// DIVISION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS

/* Divide double-digit by single-digit with remainder, assumes quotient fits in single digit
------
Input:
quo
rem
num_hi
num_lo
den
-------
Output: 
quo = (num_hi:num_lo) / den
rem = (num_hi:num_lo) % den
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes quotient fits in single digit
Assumes den != 0 */
MP_FORCE_INLINE constexpr void div_qr(
    digit_t& quo, 
    digit_t& rem, 
    const digit_t num_hi, 
    const digit_t num_lo, 
    const digit_t den) 
{
    digit2_t num = (static_cast<digit2_t>(num_hi) << RADIX) | static_cast<digit2_t>(num_lo);
    quo = static_cast<digit_t>(num / static_cast<digit2_t>(den));
    rem = static_cast<digit_t>(num % static_cast<digit2_t>(den));
}

/* Divide double-digit by single-digit, assumes quotient fits in single digit
------
Input:
quo
num_hi
num_lo
den
-------
Output: 
quo = (num_hi:num_lo) / den
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes quotient fits in single digit
Assumes den != 0 */
MP_FORCE_INLINE constexpr void div_q(
    digit_t& quo, 
    const digit_t num_hi, 
    const digit_t num_lo, 
    const digit_t den) 
{
    digit2_t num = (static_cast<digit2_t>(num_hi) << RADIX) | static_cast<digit2_t>(num_lo);
    quo = static_cast<digit_t>(num / static_cast<digit2_t>(den));
}

/* Remainder of double-digit by single-digit division
------
Input:
rem
num_hi
num_lo
den
-------
Output: 
rem = (num_hi:num_lo) % den
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes den != 0 */
MP_FORCE_INLINE constexpr void div_r(
    digit_t& rem, 
    const digit_t num_hi, 
    const digit_t num_lo, 
    const digit_t den) 
{
    digit2_t num = (static_cast<digit2_t>(num_hi) << RADIX) | static_cast<digit2_t>(num_lo);
    rem = static_cast<digit_t>(num % static_cast<digit2_t>(den));
}

/* Divide double-digit by single-digit with remainder
------
Input:
quo_hi
quo_lo
rem
num_hi
num_lo
den
-------
Output: 
quo_hi = ((num_hi:num_lo) / den) >> RADIX
quo_lo = ((num_hi:num_lo) / den) % 2^RADIX
rem = (num_hi:num_lo) % den
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes den != 0 */
MP_FORCE_INLINE constexpr void div_2qr(
    digit_t& quo_hi, 
    digit_t& quo_lo, 
    digit_t& rem, 
    const digit_t num_hi, 
    const digit_t num_lo, 
    const digit_t den) 
{
    digit2_t num = (static_cast<digit2_t>(num_hi) << RADIX) | static_cast<digit2_t>(num_lo);
    digit2_t q = num / static_cast<digit2_t>(den);
    rem = static_cast<digit_t>(num % static_cast<digit2_t>(den));
    quo_hi = static_cast<digit_t>(q >> RADIX);
    quo_lo = static_cast<digit_t>(q);
}

/* Divide double-digit by single-digit
------
Input:
quo_hi
quo_lo
num_hi
num_lo
den
-------
Output: 
quo_hi = ((num_hi:num_lo) / den) >> RADIX
quo_lo = ((num_hi:num_lo) / den) % 2^RADIX
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes den != 0 */
MP_FORCE_INLINE constexpr void div_2q(
    digit_t& quo_hi, 
    digit_t& quo_lo, 
    const digit_t num_hi, 
    const digit_t num_lo, 
    const digit_t den) 
{
    digit2_t num = (static_cast<digit2_t>(num_hi) << RADIX) | static_cast<digit2_t>(num_lo);
    digit2_t q = num / static_cast<digit2_t>(den);
    quo_hi = static_cast<digit_t>(q >> RADIX);
    quo_lo = static_cast<digit_t>(q);
}


/* Approximator of integer inverse
------
N. Möller and T. Granlund, "Improved Division by Invariant Integers," 
IEEE Transactions on Computers, vol. 60, no. 2, pp. 165-175, 2011
------
This function computes an approximation of the reciprocal of den, 
which can be used to perform fast division by den using multiplication.
------
Input:
apx
den
-------
Output: 
apx = ((2^(2*RADIX) - 1) / den) - (2^RADIX)
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes den >= 2^(RADIX - 1) */
MP_FORCE_INLINE constexpr void apx_21(
    digit_t& apx,
    const digit_t den) 
{
    digit2_t num = MAX_DIGIT2;
    // Due to the assumption den >= 2^(RADIX - 1)
    // the high word of apx is equal to 1
    // so we can just compute the low word of the result 
    apx = static_cast<digit_t>(num / static_cast<digit2_t>(den));
}


/* Divide double-digit by single-digit with remainder using approximator, assumes quotient fits in single digit
------
N. Möller and T. Granlund, "Improved Division by Invariant Integers," 
IEEE Transactions on Computers, vol. 60, no. 2, pp. 165-175, 2011
Algorithm 4
------
This function computes a division of a double digit integer by a single digit integer
by using the approximator of the reciprocal of the denominator and without direct division.
It assumes that the quotient fits in a single digit, i.e. that num_hi < den, 
and assumes that den >= 2^(RADIX - 1) so that the approximation is precise enough to yield the correct quotient.
------
Input:
quo
rem
num_hi
num_lo
den
apx
-------
Output: 
quo = (num_hi:num_lo) / den
rem = (num_hi:num_lo) % den
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes num_hi < den <=> quotient fits in single digit
Assumes den >= 2^(RADIX - 1) */
MP_FORCE_INLINE constexpr void fdiv_21_qr(
    digit_t& quo,
    digit_t& rem,
    const digit_t num_hi,
    const digit_t num_lo,
    const digit_t den,
    const digit_t apx) 
{
    digit_t quo_hi = 0, quo_lo = 0;
    digit_t temp = 0;
    digit_t carry = 0, bflag = 0, mask = 0, t0 = 0, t1 = 0;

    // Algorithm 4 in Möller and Granlund 2011
    mul(quo_hi, quo_lo, num_hi, apx);

    carry = 0;
    addc(quo_lo, carry, quo_lo, num_lo);
    addc(quo_hi, carry, quo_hi, num_hi);
    
    add(quo_hi, quo_hi, ONE);

    mul_lo(temp, quo_hi, den);
    sub(rem, num_lo, temp);


    bflag = 0; // = 1 iff rem > quo_lo
    subc(t0, bflag, quo_lo, rem);

    mask = 0 - bflag;
    sub(quo_hi, quo_hi, ONE & mask);
    add(rem, rem, den & mask);    


    bflag = 0; // = 0 iff rem ≥ den
    subc(t1, bflag, rem, den);

    mask = 0 - (bflag ^ 1);
    add(quo_hi, quo_hi, ONE & mask);
    sub(rem, rem, den & mask);

    
    quo = quo_hi;
}


/* Approximator of integer inverse
------
N. Möller and T. Granlund, "Improved Division by Invariant Integers," 
IEEE Transactions on Computers, vol. 60, no. 2, pp. 165-175, 2011
Algorithm 6
------
This function computes an approximation of the reciprocal of den, 
which can be used to perform fast division by den using multiplication.
------
Input:
apx
den_hi
den_lo
-------
Output: 
apx = ((2^(3*RADIX) - 1) / (den_hi:den_lo)) - (2^RADIX)
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes den >= 2^(2*RADIX - 1) */
MP_FORCE_INLINE constexpr void apx_32(
    digit_t& apx,
    const digit_t den_hi,
    const digit_t den_lo)
{
    digit_t v = 0, p = 0, temp0 = 0, temp1 = 0;
    digit_t carry = 0, mask1 = 0, mask2 = 0, bflag = 0, t0 = 0, t1 = 0, t2 = 0;

    apx_21(v, den_hi);
    mul_lo(p, den_hi, v);

    carry = 0;
    addc(p, carry, p, den_lo);
    
    mask1 = 0 - carry; // if overflow
    sub(v, v, ONE & mask1);

    bflag = 0; // = 0 iff p ≥ den_hi
    subc(t0, bflag, p, den_hi);

    mask2 = 0 - (bflag ^ 1);
    mask2 = mask2 & mask1;
    sub(v, v, ONE & mask2);
    sub(p, p, den_hi & mask2);
    sub(p, p, den_hi & mask1);


    mul(temp1, temp0, v, den_lo);

    carry = 0;
    addc(p, carry, p, temp1);

    mask1 = 0 - carry;
    sub(v, v, ONE & mask1);

    bflag = 0; // = 0 iff (p:temp0) ≥ (den_hi:den_lo)
    subc(t1, bflag, temp0, den_lo);
    subc(t2, bflag, p, den_hi);

    mask2 = 0 - (bflag ^ 1);
    mask2 = mask2 & mask1;
    sub(v, v, ONE & mask2);

    apx = v;
}

/* Divide triple-digit by double-digit with remainder using approximator, assumes quotient fits in single digit
N. Möller and T. Granlund, "Improved Division by Invariant Integers," 
IEEE Transactions on Computers, vol. 60, no. 2, pp. 165-175, Feb. 2011, doi: 10.1109/TC.2010.143.
Algorithm 5
------
Input:
quo
rem_hi
rem_lo
num_hi
num_mi
num_lo
den_hi
den_lo
apx
-------
Output: 
quo = (num_hi:num_mi:num_lo) / (den_hi:den_lo)
rem_hi = ((num_hi:num_mi:num_lo) % (den_hi:den_lo)) >> RADIX
rem_lo = ((num_hi:num_mi:num_lo) % (den_hi:den_lo)) % 2^RADIX
------
Notes:
THIS FUNCTION IS NOT CONSTANT TIME AND SHOULD NOT BE USED IN CRYPTOGRAPHIC CONTEXTS
Assumes (num_hi:num_mi) < (den_hi:den_lo) <=> quotient fits in single digit
Assumes den != 0 */
MP_FORCE_INLINE constexpr void fdiv_32_qr(  
    digit_t& quo,
    digit_t& rem_hi,
    digit_t& rem_lo,
    const digit_t num_hi,
    const digit_t num_mi,
    const digit_t num_lo,
    const digit_t den_hi,
    const digit_t den_lo,
    const digit_t apx)
{
    digit_t quo_hi = 0, quo_lo = 0;
    digit_t temp0 = 0, temp1 = 0;
    digit_t carry = 0, borrow = 0, bflag = 0, mask = 0;
    digit_t t0 = 0, t1 = 0;

    mul(quo_hi, quo_lo, num_hi, apx);

    carry = 0;
    addc(quo_lo, carry, quo_lo, num_mi);
    addc(quo_hi, carry, quo_hi, num_hi);

    mul_lo(temp1, quo_hi, den_hi);
    sub(rem_hi, num_mi, temp1);

    mul(temp1, temp0, quo_hi, den_lo);

    borrow = 0;
    subc(rem_lo, borrow, num_lo, temp0);
    subc(rem_hi, borrow, rem_hi, temp1);
    
    borrow = 0;
    subc(rem_lo, borrow, rem_lo, den_lo);
    subc(rem_hi, borrow, rem_hi, den_hi);
    
    add(quo_hi, quo_hi, ONE);

    bflag = 0; // = 0 iff rem_hi ≥ quo_lo
    subc(t0, bflag, rem_hi, quo_lo);
    mask = 0 - (bflag ^ 1);
    sub(quo_hi, quo_hi, ONE & mask);
    carry = 0;
    addc(rem_lo, carry, rem_lo, den_lo & mask);
    addc(rem_hi, carry, rem_hi, den_hi & mask);

    bflag = 0; // = 0 iff (rem_hi:rem_lo) ≥ (den_hi:den_lo)
    subc(t0, bflag, rem_lo, den_lo);
    subc(t1, bflag, rem_hi, den_hi);

    mask = 0 - (bflag ^ 1);

    add(quo_hi, quo_hi, ONE & mask);

    borrow = 0;
    subc(rem_lo, borrow, rem_lo, den_lo & mask);
    subc(rem_hi, borrow, rem_hi, den_hi & mask);


    quo = quo_hi;
}



/* Computes inverse modulo 2^RADIX using Newton-Raphson iteration
Precision doubles at each iteration starting from 1 bit.
------
Input:
out
in
-------
Output: 
out = in^(-1) mod 2^RADIX
------
Notes:
Assumes the input is odd (i.e., invertible modulo 2^RADIX)
Output undefined otherwise. 
TODO: check if it;s actually slower then the other function.
GODBLOT shows less instructions. */
MP_FORCE_INLINE constexpr void sinv_digit(digit_t& out, const digit_t in) {
    out = in;                       // 1 bit precision
    out = out * (2 - in * out);     // 2 bit precision
    out = out * (2 - in * out);     // 4 bit precision
    out = out * (2 - in * out);     // 8 bit precision
    out = out * (2 - in * out);     // 16 bit precision
    out = out * (2 - in * out);     // 32 bit precision
    if constexpr (RADIX == 64) {
        out = out * (2 - in * out); // 64 bit precision
    }
}  

/* Computes inverse modulo 2^RADIX using Newton-Raphson iteration
Precision doubles at each iteration starting from 4 bit.
------
Input:
out
in
-------
Output: 
out = in^(-1) mod 2^RADIX
------
Notes:
Assumes the input is odd (i.e., invertible modulo 2^RADIX)
Output undefined otherwise. 
Recall that a^(-1) ≡ a mod 8, and
a^(-1) ≡ a mod 16     if a ≡ ±1 mod 8
a^(-1) ≡ a ^ 8 mod 16 if a ≡ ±3 mod 8
Leading to below formula for 4 bit precision */
MP_FORCE_INLINE constexpr void inv_digit(digit_t& out, const digit_t in) {
    out = in ^ (((in << 1) ^ (in << 2)) & 0b1000);  // 4 bit precision
    out = out * (2 - in * out);                     // 8 bit precision
    out = out * (2 - in * out);                     // 16 bit precision
    out = out * (2 - in * out);                     // 32 bit precision
    if constexpr (RADIX == 64) {
        out = out * (2 - in * out);                 // 64 bit precision
    }
}  



} // namespace common
