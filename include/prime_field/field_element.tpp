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

    a.data = mp_sub_conditional<Prime::NWORDS, Prime::NWORDS, Prime::NWORDS>(a.data, p);
}

// Modular reduction: a = a mod p
// Input: a in [0, R - 1]
// Output: a mod p in [0, p - 1]
// Method: Call Montgomery reduction
template<typename Prime>
void reduce_full(FieldElement<Prime>& a) {
    constexpr size_t NWORDS = Prime::NWORDS;
    std::array<digit_t, 2 * NWORDS> temp_a = {};
    FieldElement<Prime> r2;
    r2.data = Prime::R2;

    for (size_t i = 0; i < NWORDS; i++)
        temp_a[i] = a.data[i];

    a.data = montgomery_reduce<Prime>(temp_a);
    mul(a, a, r2);
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
    constexpr auto& p = Prime::p;
    digit_t carry;

    carry = 0;
    for(size_t i = 0; i < NWORDS; i++)
        ADDC(out.data[i], in1.data[i], in2.data[i], carry);

    if constexpr (Prime::NBITS == NWORDS * RADIX){
        digit_t mask = 0 - carry;
        digit_t borrow = 0;
        for(size_t i = 0; i < NWORDS; i++)
            SUBC(out.data[i], out.data[i], p[i] & mask, borrow);
    }

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

    out.data = mp_sub<Prime::NWORDS, Prime::NWORDS>(p, in.data);
    reduce(out);
}

// Modular subtraction: out = (in1 - in2) mod p
// Inputs: in1, in2 in [0, p - 1]
// Output: out in [0, p - 1]
// Method: Subtracts in2 from in1
// If underflows, adds p
template<typename Prime>
void sub(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2) {
    constexpr size_t NWORDS = Prime::NWORDS;
    const auto& p = Prime::p;
    digit_t mask, borrow, carry;

    borrow = 0;
    for (size_t i = 0; i < NWORDS; i++)
        SUBC(out.data[i], in1.data[i], in2.data[i], borrow);

    mask = 0 - borrow;
    carry = 0;
    for (size_t i = 0; i < NWORDS; i++)
        ADDC(out.data[i], out.data[i], p[i] & mask, carry);
}

// Modular multiplication: c = (a * b) mod p
// Inputs: a, b in [0, p - 1] in Montgomery form
// Output: c in [0, p - 1] in Montgomery form
// Method: Interleaved Montgomery multiplication with window of size 1
// 
template<typename Prime>
void mul(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2) {
    constexpr size_t N = Prime::NWORDS;
    constexpr size_t NBITS = Prime::NBITS;
    const auto& p = Prime::p;
    constexpr digit_t nip_0 = (Prime::nip)[0];

    if constexpr (NBITS <= N * RADIX - 1) {
        std::array<digit_t, N + 1> temp_c{};
        std::array<digit_t, N + 1> temp_0{};

        for (size_t j = 0; j < N; j++) {
            temp_0 = mp_mul_digit(in1.data, in2.data[j]);
            temp_c = mp_add<N + 1, N + 1, N + 1>(temp_c, temp_0);

            digit_t q = temp_c[0] * nip_0;

            temp_0 = mp_mul_digit(p, q);
            temp_c = mp_add_and_divide<N + 1, N + 1, N + 1>(temp_c, temp_0);
        }

        out.data = mp_sub_conditional<N, N, N>(array_truncate<N>(temp_c), p);

    } else {

        std::array<digit_t, N + 2> temp_c{};
        std::array<digit_t, N + 1> temp_0{};

        for (size_t j = 0; j < N; j++) {

            temp_0 = mp_mul_digit(in1.data, in2.data[j]);
            temp_c = mp_add<N + 2, N + 2, N + 1>(temp_c, temp_0);
            
            digit_t q = temp_c[0] * nip_0;

            temp_0 = mp_mul_digit(p, q);
            temp_c = mp_add_and_divide<N + 2, N + 2, N + 1>(temp_c, temp_0);
        }

        out.data = mp_sub_conditional<N, N + 2, N>(temp_c, p);
    }
}



// Modular multiplication: c = (a * b) mod p
// Inputs: a, b in [0, p - 1] in Montgomery form
// Output: c in [0, p - 1] in Montgomery form
// Method: Interleaved Montgomery multiplication with window of size 1
// void montgomery_mul_windowed


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

            conditional_swap(t0.data, t1.data, bit ^ prevbit);

            mul(t1, t0, t1);  // t1 = t0 * t1
            mul(t0, t0, t0);  // t0 = t0 * t0

            prevbit = bit;
        }
    }

    conditional_swap(t0.data, t1.data, prevbit);

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
    FieldElement<Prime> zero = {};
    one.data = Prime::Mont_one;

    neg(non_residue, in);
    conditional_select(non_residue, non_residue, one, in == zero);

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

            conditional_swap(t0[0].data, t1[0].data, bit ^ prevbit);
            conditional_swap(t0[1].data, t1[1].data, bit ^ prevbit);

            mul_fp2(t1, t0, t1, non_residue);  // t1 = t0 * t1
            mul_fp2(t0, t0, t0, non_residue);  // t0 = t0 * t0

            prevbit = bit;
        }
    }

    conditional_swap(t0[0].data, t1[0].data, prevbit);
    conditional_swap(t0[1].data, t1[1].data, prevbit);

    conditional_select(out, t0[0], zero, in == zero);
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
void to_montgomery(FieldElement<Prime>& out, const std::array<digit_t, Prime::NWORDS>& in) {
    constexpr size_t NWORDS = Prime::NWORDS;
    FieldElement<Prime> temp, r3;
    std::array<digit_t, 2 * NWORDS> temp_in = {};
    r3.data = Prime::R3;

    for (size_t i = 0; i < NWORDS; i++)
        temp_in[i] = in[i];

    temp.data = montgomery_reduce(temp_in);
    mul(out, temp, r3);
}

template<typename Prime>
void from_montgomery(std::array<digit_t, Prime::NWORDS>& out, const FieldElement<Prime>& in) {
    FieldElement<Prime> Rinv, result;
    Rinv.data[0] = 1;

    mul(result, in, Rinv);
    out = result.data;
}

// Conditional select: out = (cond) ? in2 : in1 (constant-time)
template<typename Prime>
void conditional_select(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2, bool cond) {
    out.data = conditional_select(in1.data, in2.data, cond);
}

// Conditional swap: swaps a and b if cond != 0 (constant-time)
template<typename Prime>
void conditional_swap(FieldElement<Prime>& a, FieldElement<Prime>& b, bool cond) {
    conditional_swap(a.data, b.data, cond);
}


template<typename Prime>
bool random(FieldElement<Prime>& out) noexcept {
    constexpr size_t NWORDS = Prime::NWORDS;
    constexpr size_t LAST_BITS = (Prime::NBITS - 1) % RADIX;
    std::array<digit_t, 2 * NWORDS> temp;

    if (detail::randombytes(temp.data(),
        (2 * NWORDS) * sizeof(digit_t)) != 0)
        return false;

    temp[2 * NWORDS - 1] &= ((digit_t) 1 << LAST_BITS) - 1;

    out.data = montgomery_reduce<Prime>(temp);
    return true;
}

template<typename Prime>
FieldElement<Prime> random() {
    FieldElement<Prime> r;

    if (!random(r))
        std::terminate();

    return r;
}


} // namespace prime_field
