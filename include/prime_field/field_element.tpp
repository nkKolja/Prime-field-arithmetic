#pragma once

// Template implementation file for FieldElement
// This file is included at the end of field_element.hpp

#include <array>
#include <stdexcept>
#include "types.hpp"
#include "detail/helpers.hpp"
#include "detail/random.hpp"

namespace prime_field {

// Modular reduction: a = a mod p
// Input: a in [0, 2p - 1]  // NOTE THIS IS ASSUMED - DOES NOT WORK OTHERWISE
// Output: a in [0, p - 1]
// Method: Subtracts p and conditionally adds p back
template<typename Prime>
void reduce(FieldElement<Prime>& a) {
    constexpr auto& p = Prime::p;

    mp_sub_conditional(a.data, a.data, p);
}

// Modular reduction: a = a mod p
// Input: a in [0, R - 1]
// Output: a in [0, p - 1]
// Method: First reduces trailing bits to get a in [0, 2p - 1]
// Then calls reduce to get a in [0, p - 1]
// Assumes that R <= p * (2^(NWORDS * RADIX - NBITS) + 1)
template<typename Prime>
void reduce_full(FieldElement<Prime>& a) {
    constexpr size_t NWORDS = Prime::NWORDS;
    constexpr size_t num_trailing_bits = NWORDS * RADIX - Prime::NBITS;
    constexpr auto& TWO_POW_NBITS = Prime::TWO_POW_NBITS;

    // If no trailing bits, just reduce and return
    if constexpr (num_trailing_bits == 0) {
        reduce(a);
        return;
    }

    digit_t trailing_value = a.data[NWORDS - 1] >> (RADIX - num_trailing_bits);
    a.data[NWORDS - 1] &= ((digit_t)1 << (RADIX - num_trailing_bits)) - 1;
    
    std::array<digit_t, NWORDS> temp_0;
    std::array<digit_t, NWORDS> temp_1;
    digit_t carry;

    for (size_t i = 0; i < NWORDS; i++) {
        MUL(temp_1[i], temp_0[i], TWO_POW_NBITS[i], trailing_value);
    }

    carry = 0;
    for (size_t i = 1; i < NWORDS; i++) {
        ADDC(temp_0[i], temp_0[i], temp_1[i-1], carry);
    }

    carry = 0;
    for (size_t i = 0; i < NWORDS; i++) {
        ADDC(a.data[i], a.data[i], temp_0[i], carry);
    }

    reduce(a);
}

// Modular addition: out = (in1 + in2) mod p
// Inputs: in1, in2 in [0, p - 1]
// Output: out in [0, p - 1]
// Method: Adds two numbers
// If top digit overflows, adds it's reduce value mod p
// Then subtracts p and conditionally adds p back
template<typename Prime>
void add(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2) {
    constexpr size_t NWORDS = Prime::NWORDS;
    constexpr auto& Mont_one = Prime::Mont_one;
    digit_t mask, carry;

    carry = 0;
    for(size_t i = 0; i < NWORDS; i++)
        ADDC(out.data[i], in1.data[i], in2.data[i], carry);

    mask = 0 - carry;
    carry = 0;
    for(size_t i = 0; i < NWORDS; i++)
        ADDC(out.data[i], out.data[i], Mont_one[i] & mask, carry);
    
    reduce(out);
}

// Modular negation: out = -in mod p
// Input: in in [0, p - 1]
// Output: out in [0, p - 1]
// Method: Subtracts in from p
// Then runs reduction (to cover case in == 0)
template<typename Prime>
void neg(FieldElement<Prime>& out, const FieldElement<Prime>& in) {
    const auto& p = Prime::p;

    mp_sub_no_borrow(out.data, p, in.data);
    reduce(out);
}

// Modular subtraction: out = (in1 - in2) mod p
// Inputs: in1, in2 in [0, p - 1]
// Output: out in [0, p - 1]
// Method: Subtracts in2 from in1
// If underflows, subtracts Mont_one (which is -p mod R)
template<typename Prime>
void sub(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2) {
    constexpr size_t NWORDS = Prime::NWORDS;
    const auto& Mont_one = Prime::Mont_one;
    digit_t mask, borrow;

    borrow = 0;
    for (size_t i = 0; i < NWORDS; i++)
        SUBC(out.data[i], in1.data[i], in2.data[i], borrow);
    
    mask = 0 - borrow;
    borrow = 0;
    for (size_t i = 0; i < NWORDS; i++)
        SUBC(out.data[i], out.data[i], Mont_one[i] & mask, borrow);

    reduce_full(out);
}

// Modular multiplication: c = (a * b) mod p
// Inputs: a, b in [0, p - 1] in Montgomery form
// Output: c in [0, p - 1] in Montgomery form
// Method: Interleaved Montgomery multiplication with window of size 1
// 
template<typename Prime>
void mul(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2) {
    constexpr size_t NWORDS = Prime::NWORDS;
    constexpr size_t NBITS = Prime::NBITS;
    const auto& p = Prime::p;
    constexpr digit_t pp_0 = (Prime::pp)[0];

    if constexpr (NBITS <= NWORDS * RADIX - 1) {
        std::array<digit_t, NWORDS + 1> temp_c{};
        std::array<digit_t, NWORDS + 1> temp_0{};

        for (size_t j = 0; j < NWORDS; j++) {
            digit_mul(temp_0, in1.data, in2.data[j]);
            mp_add_no_overflow(temp_c, temp_c, temp_0);

            digit_t q = temp_c[0] * pp_0;

            digit_mul(temp_0, p, q);
            mp_add_and_divide(temp_c, temp_c, temp_0);
        }

        mp_sub_conditional(out.data, array_truncate<NWORDS>(temp_c), p);

    } else {

        std::array<digit_t, NWORDS + 2> temp_c{};
        std::array<digit_t, NWORDS + 1> temp_0{};

        for (size_t j = 0; j < NWORDS; j++) {

            digit_mul(temp_0, in1.data, in2.data[j]);
            mp_add_no_overflow(temp_c, temp_c, temp_0);
            
            digit_t q = temp_c[0] * pp_0;

            digit_mul(temp_0, p, q);
            mp_add_and_divide(temp_c, temp_c, temp_0);
        }

        mp_sub_conditional(out.data, temp_c, p);
    }
}




// Exponentiation: result = a^exp mod p (constant-time Montgomery ladder)
template<typename Prime, size_t N>
void pow(FieldElement<Prime>& result, const FieldElement<Prime>& a, const std::array<digit_t, N>& exp) {
    FieldElement<Prime> t0, t1;
    
    t0.data = Prime::Mont_one;
    t1 = a;
    
    digit_t bit = 0;
    digit_t prevbit = 0;
    
    // Process bits from most significant to least significant
    for (int i = N - 1; i >= 0; i--) {
        for (int j = 63; j >= 0; j--) {
            bit = (exp[i] >> j) & 0x01;
            
            cond_swap(t0.data, t1.data, bit ^ prevbit);
            
            mul(t1, t0, t1);  // t1 = t0 * t1
            mul(t0, t0, t0);  // t0 = t0 * t0
            
            prevbit = bit;
        }
    }
    
    cond_swap(t0.data, t1.data, prevbit);
    
    result = t0;
}

template<typename Prime>
void inv(FieldElement<Prime>& b, const FieldElement<Prime>& a) {
    // Fermat's little theorem: a^(p-2) mod p
    pow<Prime, Prime::NWORDS>(b, a, Prime::pm2);
}


// Multiplication over Fp2
// Fp[x]/(x^2 - non_residue)
// a = a[0] + a[1] * x
template<typename Prime>
void mul_fp2(
    std::array<FieldElement<Prime>, 2>& out,
    const std::array<FieldElement<Prime>, 2>& in1,
    const std::array<FieldElement<Prime>, 2>& in2,
    const FieldElement<Prime>& non_residue) {
    FieldElement<Prime> t0, t1, t2;

    add(t0, in1[0], in1[1]);
    add(t1, in2[0], in2[1]);

    mul(t2, t0, t1);

    mul(t0, in1[0], in2[0]);
    mul(t1, in1[1], in2[1]);

    sub(t2, t2, t0);
    sub(out[1], t2, t1);

    mul(t1, t1, non_residue);

    add(out[0], t0, t1);
}

// Cipolla's square root finding algorithm
template<typename Prime>
void sqrt(FieldElement<Prime>& out, const FieldElement<Prime>& in) {
    FieldElement<Prime> a = {};
    FieldElement<Prime> non_residue = {};
    FieldElement<Prime> one = {};
    one.data = Prime::Mont_one;

    neg(non_residue, in);

    while (legendre(non_residue) != -1) {
        add(a, a, one);
        add(non_residue, non_residue, a);
        add(non_residue, non_residue, a);
        sub(non_residue, non_residue, one);
    }

    std::array<FieldElement<Prime>, 2> t0 = {};
    std::array<FieldElement<Prime>, 2> t1 = {};
    std::array<digit_t, Prime::NWORDS> exp = Prime::pp1_half;

    t0[0].data = Prime::Mont_one;
    t1[0] = a;
    t1[1] = one;


    digit_t bit = 0;
    digit_t prevbit = 0;
    
    for (int i = Prime::NWORDS - 1; i >= 0; i--) {
        for (int j = RADIX - 1; j >= 0; j--) {
            bit = (exp[i] >> j) & 0x01;
            
            cond_swap(t0[0].data, t1[0].data, bit ^ prevbit);
            cond_swap(t0[1].data, t1[1].data, bit ^ prevbit);
            
            mul_fp2(t1, t0, t1, non_residue);  // t1 = t0 * t1
            mul_fp2(t0, t0, t0, non_residue);  // t0 = t0 * t0
            
            prevbit = bit;
        }
    }
    
    cond_swap(t0[0].data, t1[0].data, prevbit);
    cond_swap(t0[1].data, t1[1].data, prevbit);
    
    out = t0[0];
}

// Returns the Legendre symbol (a|p)
// 1 if a is quadratic residue mod p
// -1 if a is non-residue mod p
// 0 if a == 0 mod p
template<typename Prime>
int legendre(const FieldElement<Prime>& a) {
    // Legendre symbol: a^((p-1)/2) mod p
    FieldElement<Prime> result;
    pow<Prime, Prime::NWORDS>(result, a, Prime::pm1_half);
    
    FieldElement<Prime> one(1);
    FieldElement<Prime> minus_one = -one;
    
    int is_one = result == one;
    int is_minus_one = result == minus_one;
    
    int ret_one = 1 & -is_one;
    int ret_minus_one = -is_minus_one;
    int retval = ret_one | ret_minus_one;
    
    return retval;
}

template<typename Prime>
bool random(FieldElement<Prime>& out) noexcept {
    if (detail::randombytes(out.data.data(),
        Prime::NWORDS * sizeof(digit_t)) != 0)
        return false;

    reduce_full(out);
    return true;
}

template<typename Prime>
FieldElement<Prime> random() {
    FieldElement<Prime> r;

    if (!random(r))
        std::terminate();

    return r;
}


template<typename Prime>
FieldElement<Prime> to_montgomery(const std::array<digit_t, Prime::NWORDS>& value) {
    FieldElement<Prime> result, in, r2;
    
    in.data = value;
    r2.data = Prime::R2;
    mul(result, in, r2);
    return result;
}

template<typename Prime>
std::array<digit_t, Prime::NWORDS> from_montgomery(const FieldElement<Prime>& a) {
    FieldElement<Prime> one;
    one.data[0] = 1;
    FieldElement<Prime> result;
    mul(result, a, one);
    return result.data;
}

} // namespace prime_field
