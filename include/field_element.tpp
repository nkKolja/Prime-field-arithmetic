#pragma once

// Template implementation file for FieldElement
// This file is included at the end of field_element.hpp

#include <array>
#include "types.hpp"
#include "helpers.hpp"

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
    digit_t mask, borrow, carry;

    // If no trailing bits, just reduce and return
    if constexpr (num_trailing_bits == 0) {
        reduce(a);
        return;
    }

    digit_t trailing_value = a.data[NWORDS - 1] >> (RADIX - num_trailing_bits);
    a.data[NWORDS - 1] &= ((digit_t)1 << (RADIX - num_trailing_bits)) - 1;
    
    std::array<digit_t, NWORDS> temp_0;
    std::array<digit_t, NWORDS> temp_1;

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
    digit_t mask, carry, borrow;

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

        mp_sub_conditional_NM(out.data, temp_c, p);
    }
}


// Exponentiation: result = a^exp mod p
template<typename Prime>
void pow(FieldElement<Prime>& result, const FieldElement<Prime>& a, const std::array<digit_t, Prime::NWORDS>& exp) {
    constexpr size_t N = Prime::NWORDS;
    
    // Initialize result to 1 (in Montgomery form: R mod p)
    result.data = Prime::Mont_one;
    
    FieldElement<Prime> base = a;
    
    // Square-and-multiply
    for (size_t i = 0; i < N; i++) {
        digit_t e = exp[i];
        for (size_t j = 0; j < 64; j++) {
            if (e & 1) {
                mul(result, base, result);
            }
            mul(base, base, base);
            e >>= 1;
        }
    }
}

template<typename Prime>
void inv(FieldElement<Prime>& b, const FieldElement<Prime>& a) {
    // Fermat's little theorem: a^(p-2) mod p
    constexpr size_t N = Prime::NWORDS;
    const auto& p = Prime::p;
    
    // Compute p - 2
    std::array<digit_t, N> p_minus_2 = p;
    digit_t borrow = 2;
    for (size_t i = 0; i < N; i++) {
        uint128_t diff = static_cast<uint128_t>(p_minus_2[i]) - borrow;
        p_minus_2[i] = static_cast<digit_t>(diff);
        borrow = static_cast<digit_t>(diff >> 127);
        if (borrow == 0) break;
    }
    
    pow(b, a, p_minus_2);
}

template<typename Prime>
void sqrt(FieldElement<Prime>& b, const FieldElement<Prime>& a) {
    // TODO: Implement Tonelli-Shanks
    b = a;
}

// Returns the Legendre symbol (a|p)
// 1 if a is quadratic residue mod p
// -1 if a is non-residue mod p
// 0 if a == 0 mod p
template<typename Prime>
int legendre(const FieldElement<Prime>& a) {
    // Legendre symbol: a^((p-1)/2) mod p
    constexpr size_t N = Prime::NWORDS;
    const auto& p = Prime::p;

    // Compute (p - 1) / 2
    std::array<digit_t, N> leg_exp = p;
    for (size_t i = 0; i < N; i++) {
        leg_exp[i] >>= 1;
    }
    for (size_t i = 0; i < N - 1; i++) {
        leg_exp[i] |= (p[i + 1] & 0x01) << (RADIX - 1);
    }

    FieldElement<Prime> result;
    pow(result, a, leg_exp);
    
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
FieldElement<Prime> random() {
    // TODO: Implement with secure RNG
    // For now, placeholder
    return FieldElement<Prime>();
}

template<typename Prime>
void conditional_select(const FieldElement<Prime>& a, const FieldElement<Prime>& b, FieldElement<Prime>& c, uint8_t cond) {
    constexpr size_t N = Prime::NWORDS;
    digit_t mask = static_cast<digit_t>(-(digit_t)cond);
    
    for (size_t i = 0; i < N; i++) {
        c.data[i] = (a.data[i] & ~mask) | (b.data[i] & mask);
    }
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
