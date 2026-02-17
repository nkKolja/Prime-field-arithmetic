#include <iostream>
#include <cstring>
#include <cstdint>
#include <vector>
#include "../include/common/primitives.hpp"
#include "../include/common/types.hpp"

using namespace common;

// Test configuration
#define ADDC_TESTS  6
#define ADD_TESTS   3
#define SUBC_TESTS  7
#define SUB_TESTS   3
#define MUL_TESTS   5
#define MULHI_TESTS 3
#define MULLO_TESTS 3
#define DIVQR_TESTS 5
#define DIVQ_TESTS  3
#define DIVR_TESTS  4
#define DIV2QR_TESTS 3
#define DIV2Q_TESTS 3
#define APX21_TESTS 3
#define FDIV21_TESTS 12
#define APX32_TESTS 4
#define FDIV32_TESTS 9

#define NUM_TESTS   16
#define TESTS_PAD   12

#define TESTS_LEN(x) (  (x) == 0  ? ADDC_TESTS   : \
                        (x) == 1  ? ADD_TESTS    : \
                        (x) == 2  ? SUBC_TESTS   : \
                        (x) == 3  ? SUB_TESTS    : \
                        (x) == 4  ? MUL_TESTS    : \
                        (x) == 5  ? MULHI_TESTS  : \
                        (x) == 6  ? MULLO_TESTS  : \
                        (x) == 7  ? DIVQR_TESTS  : \
                        (x) == 8  ? DIVQ_TESTS   : \
                        (x) == 9  ? DIVR_TESTS   : \
                        (x) == 10 ? DIV2QR_TESTS : \
                        (x) == 11 ? DIV2Q_TESTS  : \
                        (x) == 12 ? APX21_TESTS  : \
                        (x) == 13 ? FDIV21_TESTS : \
                        (x) == 14 ? APX32_TESTS  : \
                        (x) == 15 ? FDIV32_TESTS : -1)

// Test state
static bool tests[NUM_TESTS][256] = {{0}};
static bool function_failed[NUM_TESTS] = {false};
static bool function_skipped[NUM_TESTS] = {false};
struct FailureInfo {
    std::string name;
    int line;
};
static std::vector<FailureInfo> failed_tests;

#define PASS(skipped, failed) ((skipped) ? "\033[90m-\033[0m" : ((failed) ? "\033[31m✗\033[0m" : "\033[0;32m✔\033[0m"))

const char* pass_check(int fn_idx, int n) {
    static char buffer[256] = "";
    buffer[0] = '\0';
    for(int i = 0; i < TESTS_PAD - n; i++)
        strcat(buffer, " ");
    for(int i = 0; i < n; i++)
        strcat(buffer, PASS(function_skipped[fn_idx], tests[fn_idx][i]));
    return buffer;
}

#define TEST(fn_idx, test_idx, name, condition) \
    do { \
        if (function_skipped[fn_idx]) break; \
        if (!(condition)) { \
            tests[fn_idx][test_idx] = true; \
            function_failed[fn_idx] = true; \
            failed_tests.push_back({name, __LINE__}); \
        } \
    } while(0)

#define SKIP_IF_FAILED(...) \
    do { \
        int deps[] = {__VA_ARGS__}; \
        for (size_t i = 0; i < sizeof(deps)/sizeof(deps[0]); i++) { \
            if (function_failed[deps[i]]) { \
                function_skipped[fn_idx] = true; \
                return; \
            } \
        } \
    } while(0)

// Individual test functions

// Test addc: addition with carry-in and carry-out
// Tests basic addition, overflow cases, and carry propagation
void test_addc() {
    const int fn_idx = 0;
    digit_t out, carry;
    
    carry = 0;
    addc(out, carry, 5, 10);
    TEST(fn_idx, 0, "addc: 5 + 10 + 0", out == 15 && carry == 0);
    
    carry = 1;
    addc(out, carry, 5, 10);
    TEST(fn_idx, 1, "addc: 5 + 10 + 1", out == 16 && carry == 0);
    
    carry = 0;
    addc(out, carry, MAX_DIGIT, 1);
    TEST(fn_idx, 2, "addc: MAX + 1 + 0", out == 0 && carry == 1);
    
    carry = 1;
    addc(out, carry, MAX_DIGIT, 0);
    TEST(fn_idx, 3, "addc: MAX + 0 + 1", out == 0 && carry == 1);
    
    carry = 1;
    addc(out, carry, MAX_DIGIT, MAX_DIGIT);
    TEST(fn_idx, 4, "addc: MAX + MAX + 1", out == MAX_DIGIT && carry == 1);
    
    carry = 0;
    addc(out, carry, 0, 0);
    TEST(fn_idx, 5, "addc: 0 + 0 + 0", out == 0 && carry == 0);
}

// Test add: simple addition without carry handling
// Tests basic addition and overflow wrapping behavior
void test_add() {
    const int fn_idx = 1;
    digit_t out;
    
    add(out, 5, 10);
    TEST(fn_idx, 0, "add: 5 + 10", out == 15);
    
    add(out, MAX_DIGIT, 1);
    TEST(fn_idx, 1, "add: MAX + 1 (overflow)", out == 0);
    
    add(out, 0, 0);
    TEST(fn_idx, 2, "add: 0 + 0", out == 0);
}

// Test subc: subtraction with borrow-in and borrow-out
// Tests basic subtraction, underflow cases, and borrow propagation
void test_subc() {
    const int fn_idx = 2;
    digit_t out, borrow;

    borrow = 0;
    subc(out, borrow, 10, 5);
    TEST(fn_idx, 0, "subc: 10 - 5 - 0", out == 5 && borrow == 0);

    borrow = 1;
    subc(out, borrow, 10, 5);
    TEST(fn_idx, 1, "subc: 10 - 5 - 1", out == 4 && borrow == 0);

    borrow = 0;
    subc(out, borrow, 5, 10);
    TEST(fn_idx, 2, "subc: 5 - 10 - 0", out == MAX_DIGIT - 4 && borrow == 1);

    borrow = 0;
    subc(out, borrow, 0, 0);
    TEST(fn_idx, 3, "subc: 0 - 0 - 0", out == 0 && borrow == 0);

    borrow = 0;
    subc(out, borrow, 0, 1);
    TEST(fn_idx, 4, "subc: 0 - 1 - 0", out == MAX_DIGIT && borrow == 1);

    borrow = 0;
    subc(out, borrow, MAX_DIGIT, MAX_DIGIT);
    TEST(fn_idx, 5, "subc: MAX - MAX - 0", out == 0 && borrow == 0);

    borrow = 1;
    subc(out, borrow, MAX_DIGIT, MAX_DIGIT);
    TEST(fn_idx, 6, "subc: MAX - MAX - 1", out == MAX_DIGIT && borrow == 1);
}

// Test sub: simple subtraction without borrow handling
// Tests basic subtraction and underflow wrapping behavior
void test_sub() {
    const int fn_idx = 3;
    digit_t out;
    
    sub(out, 10, 5);
    TEST(fn_idx, 0, "sub: 10 - 5", out == 5);
    
    sub(out, 5, 10);
    TEST(fn_idx, 1, "sub: 5 - 10 (underflow)", out == MAX_DIGIT - 4);
    
    sub(out, 0, 0);
    TEST(fn_idx, 2, "sub: 0 - 0", out == 0);
}

// Test mul: full multiplication producing double-width result (hi:lo)
// Tests basic multiplication, edge cases with 0/1/MAX_DIGIT, and overflow into high word
void test_mul() {
    const int fn_idx = 4;
    digit_t hi, lo;
    
    mul(hi, lo, 5, 10);
    TEST(fn_idx, 0, "mul: 5 * 10", hi == 0 && lo == 50);
    
    mul(hi, lo, MAX_DIGIT, 0);
    TEST(fn_idx, 1, "mul: MAX * 0", hi == 0 && lo == 0);
    
    mul(hi, lo, MAX_DIGIT, 1);
    TEST(fn_idx, 2, "mul: MAX * 1", hi == 0 && lo == MAX_DIGIT);
    
    digit_t large = (digit_t)1 << (RADIX / 2);
    mul(hi, lo, large, large);
    TEST(fn_idx, 3, "mul: 2^(R/2) * 2^(R/2)", hi == 1 && lo == 0);
    
    mul(hi, lo, MAX_DIGIT, 2);
    TEST(fn_idx, 4, "mul: MAX * 2", hi == 1 && lo == MAX_DIGIT - 1);
}

// Test mul_hi: extract only the high word of the multiplication result
// Verifies high word extraction matches full multiplication result
void test_mul_hi() {
    const int fn_idx = 5;
    SKIP_IF_FAILED(4); // Depends on mul
    
    digit_t out;
    digit_t large = (digit_t)1 << (RADIX / 2);
    
    mul_hi(out, 5, 10);
    TEST(fn_idx, 0, "mul_hi: 5 * 10", out == 0);
    
    mul_hi(out, large, large);
    TEST(fn_idx, 1, "mul_hi: 2^(R/2) * 2^(R/2)", out == 1);
    
    mul_hi(out, MAX_DIGIT, 2);
    TEST(fn_idx, 2, "mul_hi: MAX * 2", out == 1);
}

// Test mul_lo: extract only the low word of the multiplication result
// Verifies low word extraction matches full multiplication result
void test_mul_lo() {
    const int fn_idx = 6;
    SKIP_IF_FAILED(4); // Depends on mul
    
    digit_t out;
    digit_t large = (digit_t)1 << (RADIX / 2);
    
    mul_lo(out, 5, 10);
    TEST(fn_idx, 0, "mul_lo: 5 * 10", out == 50);
    
    mul_lo(out, large, large);
    TEST(fn_idx, 1, "mul_lo: 2^(R/2) * 2^(R/2)", out == 0);
    
    mul_lo(out, MAX_DIGIT, 2);
    TEST(fn_idx, 2, "mul_lo: MAX * 2", out == MAX_DIGIT - 1);
}

// Test div_qr: division of double-width numerator by single-width denominator
// Produces both quotient and remainder. Tests basic division and edge cases.
void test_div_qr() {
    const int fn_idx = 7;
    digit_t quo, rem;
    
    div_qr(quo, rem, 0, 50, 10);
    TEST(fn_idx, 0, "div_qr: 50 / 10", quo == 5 && rem == 0);
    
    div_qr(quo, rem, 0, 53, 10);
    TEST(fn_idx, 1, "div_qr: 53 / 10", quo == 5 && rem == 3);
    
    div_qr(quo, rem, 0, MAX_DIGIT, 1);
    TEST(fn_idx, 2, "div_qr: MAX / 1", quo == MAX_DIGIT && rem == 0);
    
    digit_t expected_quo = (MAX_DIGIT >> 1) + 1;
    div_qr(quo, rem, 1, 0, 2);
    TEST(fn_idx, 3, "div_qr: (1:0) / 2", quo == expected_quo && rem == 0);
    
    div_qr(quo, rem, 0, 0, 123);
    TEST(fn_idx, 4, "div_qr: 0 / 123", quo == 0 && rem == 0);
}

// Test div_q: division producing only quotient (no remainder)
// Verifies quotient-only variant matches full division
void test_div_q() {
    const int fn_idx = 8;
    SKIP_IF_FAILED(7); // Depends on div_qr
    
    digit_t quo;
    digit_t expected_quo = (MAX_DIGIT >> 1) + 1;
    
    div_q(quo, 0, 50, 10);
    TEST(fn_idx, 0, "div_q: 50 / 10", quo == 5);
    
    div_q(quo, 0, 53, 10);
    TEST(fn_idx, 1, "div_q: 53 / 10", quo == 5);
    
    div_q(quo, 1, 0, 2);
    TEST(fn_idx, 2, "div_q: (1:0) / 2", quo == expected_quo);
}

// Test div_r: division producing only remainder (no quotient)
// Verifies remainder-only variant matches full division
void test_div_r() {
    const int fn_idx = 9;
    SKIP_IF_FAILED(7); // Depends on div_qr
    
    digit_t rem;
    
    div_r(rem, 0, 50, 10);
    TEST(fn_idx, 0, "div_r: 50 / 10", rem == 0);
    
    div_r(rem, 0, 53, 10);
    TEST(fn_idx, 1, "div_r: 53 / 10", rem == 3);
    
    div_r(rem, 1, 0, 2);
    TEST(fn_idx, 2, "div_r: (1:0) / 2", rem == 0);
    
    div_r(rem, 1, 5, 7);
    digit_t res_rem = (RADIX == 64) ? 0 : 2;
    TEST(fn_idx, 3, "div_r: (1:5) / 7", rem == res_rem);
}

// Test div_2qr: division producing double-width quotient and single-width remainder
// Handles cases where quotient doesn't fit in single digit
void test_div_2qr() {
    const int fn_idx = 10;
    SKIP_IF_FAILED(7); // Depends on div_qr
    
    digit_t quo_hi, quo_lo, rem;
    
    div_2qr(quo_hi, quo_lo, rem, 0, 50, 10);
    TEST(fn_idx, 0, "div_2qr: 50 / 10", quo_hi == 0 && quo_lo == 5 && rem == 0);
    
    div_2qr(quo_hi, quo_lo, rem, MAX_DIGIT, MAX_DIGIT, 1);
    TEST(fn_idx, 1, "div_2qr: (MAX:MAX) / 1", quo_hi == MAX_DIGIT && quo_lo == MAX_DIGIT && rem == 0);
    
    div_2qr(quo_hi, quo_lo, rem, MAX_DIGIT, MAX_DIGIT, 2);
    TEST(fn_idx, 2, "div_2qr: (MAX:MAX) / 2", quo_hi == MAX_DIGIT >> 1 && quo_lo == MAX_DIGIT && rem == 1);
}

// Test div_2q: division producing double-width quotient only (no remainder)
// Verifies double-width quotient-only variant matches full division
void test_div_2q() {
    const int fn_idx = 11;
    SKIP_IF_FAILED(7); // Depends on div_qr
    
    digit_t quo_hi, quo_lo;
    
    div_2q(quo_hi, quo_lo, 0, 50, 10);
    TEST(fn_idx, 0, "div_2q: 50 / 10", quo_hi == 0 && quo_lo == 5);

    div_2q(quo_hi, quo_lo, MAX_DIGIT, MAX_DIGIT, 1);
    TEST(fn_idx, 1, "div_2q: (MAX:MAX) / 1", quo_hi == MAX_DIGIT && quo_lo == MAX_DIGIT);

    div_2q(quo_hi, quo_lo, MAX_DIGIT, MAX_DIGIT, 2);
    TEST(fn_idx, 2, "div_2q: (MAX:MAX) / 2", quo_hi == MAX_DIGIT >> 1 && quo_lo == MAX_DIGIT);
}

// Test apx_21: compute 2:1 division approximator (Möller-Granlund Algorithm 4)
// Approximator is used for fast division of double-width by single-width numbers
// Verifies apx = floor((2^(2*R) - 1) / den) - 2^R for normalized denominators
void test_apx_21() {
    const int fn_idx = 12;
    SKIP_IF_FAILED(7); // Depends on div_qr
    
    digit_t apx, res_apx, den;
    
    den = (digit_t)1 << (RADIX - 1);
    apx_21(apx, den);
    res_apx = MAX_DIGIT;
    TEST(fn_idx, 0, "apx_21: approximator for 2^(R-1)", apx == res_apx);

    den = ((digit_t)1 << (RADIX - 1)) + 7;
    apx_21(apx, den);
    res_apx = (RADIX == 64) ? 18446744073709551588ULL : 4294967268ULL;
    TEST(fn_idx, 1, "apx_21: approximator for 2^(R-1) + 7", apx == res_apx);

    den = MAX_DIGIT;
    apx_21(apx, den);
    res_apx = ONE;
    TEST(fn_idx, 2, "apx_21: approximator for 2^(R)-1", apx == res_apx);
}

// Test fdiv_21_qr: fast 2:1 division using precomputed approximator (Algorithm 5)
// Divides (num_hi:num_lo) by den using apx, producing quotient and remainder
// Verifies results match standard division for normalized denominators
void test_fdiv_21_qr() {
    const int fn_idx = 13;
    SKIP_IF_FAILED(4, 0, 1, 2, 3, 12); // Depends on mul, addc, add, subc, sub, apx_21
    
    digit_t quo, rem, apx;
    digit_t num_hi, num_lo, quo_check, rem_check;
    
    digit_t den = (digit_t)1 << (RADIX - 1);
    apx_21(apx, den);

    num_hi = 0;
    num_lo = 0;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 0, "fdiv_21_qr: quotient match case 0", quo == quo_check && rem == rem_check);

    num_hi = 0;
    num_lo = den;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 1, "fdiv_21_qr: quotient match case 1", quo == quo_check && rem == rem_check);
    
    num_hi = 5;
    num_lo = 577;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 2, "fdiv_21_qr: quotient match case 2", quo == quo_check && rem == rem_check);
    
    num_hi = den - ONE;
    num_lo = MAX_DIGIT;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 3, "fdiv_21_qr: quotient match case 3", quo == quo_check && rem == rem_check);



    den = ((digit_t)1 << (RADIX - 1)) + 13;
    apx_21(apx, den);

    num_hi = 0;
    num_lo = 0;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 4, "fdiv_21_qr: quotient match case 4", quo == quo_check && rem == rem_check);

    num_hi = 0;
    num_lo = den;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 5, "fdiv_21_qr: quotient match case 5", quo == quo_check && rem == rem_check);

    num_hi = 15;
    num_lo = 157;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 6, "fdiv_21_qr: quotient match case 6", quo == quo_check && rem == rem_check);
    
    num_hi = den - ONE;
    num_lo = MAX_DIGIT;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 7, "fdiv_21_qr: quotient match case 7", quo == quo_check && rem == rem_check);



    den = MAX_DIGIT;
    apx_21(apx, den);

    num_hi = 0;
    num_lo = 0;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 8, "fdiv_21_qr: quotient match case 8", quo == quo_check && rem == rem_check);

    num_hi = 0;
    num_lo = den;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 9, "fdiv_21_qr: quotient match case 9", quo == quo_check && rem == rem_check);

    num_hi = 65;
    num_lo = 67;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 10, "fdiv_21_qr: quotient match case 10", quo == quo_check && rem == rem_check);
    
    num_hi = den - ONE;
    num_lo = MAX_DIGIT;
    fdiv_21_qr(quo, rem, num_hi, num_lo, den, apx);
    div_qr(quo_check, rem_check, num_hi, num_lo, den);
    TEST(fn_idx, 11, "fdiv_21_qr: quotient match case 11", quo == quo_check && rem == rem_check);
}

// Test apx_32: compute 3:2 division approximator (Möller-Granlund Algorithm 6)
// Approximator is used for fast division of triple-width by double-width numbers
// Verifies apx = floor((2^(3*R) - 1) / (den_hi:den_lo)) - 2^R for normalized denominators
void test_apx_32() {
    const int fn_idx = 14;
    SKIP_IF_FAILED(12, 6, 0, 2, 3, 4); // Depends on apx_21, mul_lo, addc, subc, sub, mul
    
    digit_t apx, res_apx;
    digit_t den_hi, den_lo;

    den_hi = (digit_t)1 << (RADIX - 1);
    den_lo = 0;
    apx_32(apx, den_hi, den_lo);
    res_apx = MAX_DIGIT;
    TEST(fn_idx, 0, "apx_32: approximator for 2^(2*R-1)", apx == res_apx);

    den_hi = ((digit_t)1 << (RADIX - 1))+ 7;
    den_lo = 7;
    apx_32(apx, den_hi, den_lo);
    res_apx = (RADIX == 64) ? 18446744073709551588ULL : 4294967268ULL;
    TEST(fn_idx, 1, "apx_32: approximator for 2^(2*R-1) + 7*2^R + 7", apx == res_apx);

    den_hi = ((digit_t)1 << (RADIX - 1)) + 13;
    den_lo = 68723;
    apx_32(apx, den_hi, den_lo);
    res_apx = (RADIX == 64) ? 18446744073709551563ULL : 4294967243ULL;
    TEST(fn_idx, 2, "apx_32: approximator for 2^(2*R-1) + 13*2^R + 68723", apx == res_apx);

    den_hi = MAX_DIGIT;
    den_lo = MAX_DIGIT;
    apx_32(apx, den_hi, den_lo);
    res_apx = 0ULL;
    TEST(fn_idx, 3, "apx_32: approximator for 2^(2*R) - 1", apx == res_apx);
    
}

// Test fdiv_32_qr: fast 3:2 division using precomputed approximator
// Divides (num_hi:num_mi:num_lo) by (den_hi:den_lo) using apx
// Verifies quo * den + rem == num and rem < den for correctness
void test_fdiv_32_qr() {
    const int fn_idx = 15;
    SKIP_IF_FAILED(14, 4, 0, 6, 1, 3, 2); // Depends on apx_32, mul, addc, mul_lo, add, sub, subc
    
    digit_t quo, rem_hi, rem_lo, apx;
    digit_t den_hi, den_lo;
    digit_t num_hi, num_mi, num_lo;
    digit_t res_quo, res_rem_hi, res_rem_lo;
    digit_t borrow;
    
    // Test with den = 2^(2*R-1)
    den_hi = (digit_t)1 << (RADIX - 1);
    den_lo = 0;
    apx_32(apx, den_hi, den_lo);
    
    // num = 0 should yield quo = 0 and rem = 0
    num_hi = 0;
    num_mi = 0;
    num_lo = 0;
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = 0;
    res_rem_hi = 0;
    res_rem_lo = 0;
    TEST(fn_idx, 0, "fdiv_32_qr: case 0 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);
    
    // num = den should yield quo = 1 and rem = 0
    num_hi = 0;
    num_mi = den_hi;
    num_lo = den_lo;
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = 1;
    res_rem_hi = 0;
    res_rem_lo = 0;
    TEST(fn_idx, 1, "fdiv_32_qr: case 1 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);
    
    // num = 2^RADIX * den - 1 should yield quo = 2^RADIX - 1 and rem = den - 1
    num_hi = den_hi;
    num_mi = den_lo;
    num_lo = 0;
    borrow = 0;
    subc(num_lo, borrow, num_lo, ONE);
    subc(num_mi, borrow, num_mi, 0);
    subc(num_hi, borrow, num_hi, 0);
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = MAX_DIGIT;
    borrow = 0;
    subc(res_rem_lo, borrow, den_lo, ONE);
    subc(res_rem_hi, borrow, den_hi, 0);
    TEST(fn_idx, 2, "fdiv_32_qr: case 2 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);



    // Test with den = 2^(2*R-1) + 7 * 2^R + 35
    den_hi = ((digit_t)1 << (RADIX - 1)) + 7;
    den_lo = 35;
    apx_32(apx, den_hi, den_lo);

    // num = 0 should yield quo = 0 and rem = 0
    num_hi = 0;
    num_mi = 0;
    num_lo = 0;
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = 0;
    res_rem_hi = 0;
    res_rem_lo = 0;
    TEST(fn_idx, 3, "fdiv_32_qr: case 3 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);
    
    // num = den should yield quo = 1 and rem = 0
    num_hi = 0;
    num_mi = den_hi;
    num_lo = den_lo;
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = 1;
    res_rem_hi = 0;
    res_rem_lo = 0;
    TEST(fn_idx, 4, "fdiv_32_qr: case 4 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);
    
    // num = 2^RADIX * den - 1 should yield quo = 2^RADIX - 1 and rem = den - 1
    num_hi = den_hi;
    num_mi = den_lo;
    num_lo = 0;
    borrow = 0;
    subc(num_lo, borrow, num_lo, ONE);
    subc(num_mi, borrow, num_mi, 0);
    subc(num_hi, borrow, num_hi, 0);
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = MAX_DIGIT;
    borrow = 0;
    subc(res_rem_lo, borrow, den_lo, ONE);
    subc(res_rem_hi, borrow, den_hi, 0);
    TEST(fn_idx, 5, "fdiv_32_qr: case 5 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);



    // Test with den = MAX_DIGIT:MAX_DIGIT (i.e., 2^(2*R) - 1)
    den_hi = MAX_DIGIT;
    den_lo = MAX_DIGIT;
    apx_32(apx, den_hi, den_lo);

    // num = 0 should yield quo = 0 and rem = 0
    num_hi = 0;
    num_mi = 0;
    num_lo = 0;
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = 0;
    res_rem_hi = 0;
    res_rem_lo = 0;
    TEST(fn_idx, 6, "fdiv_32_qr: case 6 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);
    
    // num = den should yield quo = 1 and rem = 0
    num_hi = 0;
    num_mi = den_hi;
    num_lo = den_lo;
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = 1;
    res_rem_hi = 0;
    res_rem_lo = 0;
    TEST(fn_idx, 7, "fdiv_32_qr: case 7 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);
    
    // num = 2^RADIX * den - 1 should yield quo = 2^RADIX - 1 and rem = den - 1
    num_hi = den_hi;
    num_mi = den_lo;
    num_lo = 0;
    borrow = 0;
    subc(num_lo, borrow, num_lo, ONE);
    subc(num_mi, borrow, num_mi, 0);
    subc(num_hi, borrow, num_hi, 0);
    fdiv_32_qr(quo, rem_hi, rem_lo, num_hi, num_mi, num_lo, den_hi, den_lo, apx);
    res_quo = MAX_DIGIT;
    borrow = 0;
    subc(res_rem_lo, borrow, den_lo, ONE);
    subc(res_rem_hi, borrow, den_hi, 0);
    TEST(fn_idx, 8, "fdiv_32_qr: case 8 verification", quo == res_quo && rem_hi == res_rem_hi && rem_lo == res_rem_lo);

}

int run_tests() {
    const char *function_names[] = {
        "addc  ", "add   ", "subc  ", "sub   ", 
        "mul   ", "mul_hi", "mul_lo", "div_qr",
        "div_q ", "div_r ", "div2qr", "div2q ",
        "apx21 ", "fdiv21", "apx32 ", "fdiv32"
    };
    
    // Run all test functions
    test_addc();
    test_add();
    test_subc();
    test_sub();
    test_mul();
    test_mul_hi();
    test_mul_lo();
    test_div_qr();
    test_div_q();
    test_div_r();
    test_div_2qr();
    test_div_2q();
    test_apx_21();
    test_fdiv_21_qr();
    test_apx_32();
    test_fdiv_32_qr();
    
    // Print compact results
    for(int j = 0; j < NUM_TESTS; j++) {
        std::cout << function_names[j] << "   " << pass_check(j, TESTS_LEN(j)) << "\n";
    }
    std::cout << "\n";
    
    // Print failures after results
    if (!failed_tests.empty()) {
        std::cerr << "\033[31m━━━ Failed Tests ━━━\033[0m\n";
        for (const auto& failure : failed_tests) {
            std::cerr << "  \033[31m✗\033[0m " << failure.name 
                     << " \033[90m(line " << failure.line << ")\033[0m\n";
        }
        std::cerr << "\033[31m━━━━━━━━━━━━━━━━━\033[0m\n\n";
    }
    
    // Count total and failed tests
    int total_tests = 0;
    int failed_tests = 0;
    int skipped_fns = 0;
    
    for(int i = 0; i < NUM_TESTS; i++) {
        if (function_skipped[i]) {
            skipped_fns++;
            continue;
        }
        for(int j = 0; j < TESTS_LEN(i); j++) {
            total_tests++;
            if (tests[i][j]) failed_tests++;
        }
    }
    
    if(failed_tests == 0 && skipped_fns == 0) {
        std::cout << "✔ All " << total_tests << " tests passed!\n\n";
        return 0;
    } else {
        if (failed_tests > 0) {
            std::cout << "✗ " << failed_tests << " of " << total_tests << " tests failed\n";
        }
        if (skipped_fns > 0) {
            std::cout << "\033[90m" << skipped_fns << " function(s) skipped due to dependency failures\033[0m\n";
        }
        std::cout << "\n";
        return 1;
    }
}

int main() {
    std::cout << "\n";
    std::cout << "=====================\n";
    std::cout << "Testing Common Primitives\n";
    std::cout << "RADIX = " << RADIX << " bits\n";
    std::cout << "=====================\n\n";
    
    int result = run_tests();
    
    return result;
}
