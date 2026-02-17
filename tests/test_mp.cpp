#include <iostream>
#include <cstring>
#include <cstdint>
#include <vector>
#include "../include/mp/mp.hpp"
#include "../include/common/types.hpp"

using namespace common;
using namespace mp;

// Test configuration
#define MP_ADD_TESTS         8
#define MP_ADDC_TESTS        6
#define MP_ADDC_DIGIT_TESTS  5
#define MP_ADD_DIV_TESTS     4
#define MP_ADDC_DIV_TESTS    4
#define MP_SUB_TESTS         8
#define MP_SUBC_TESTS        6
#define MP_SUB_COND_TESTS    5
#define MP_SUBC_COND_TESTS   5
#define MP_MUL_TESTS         8
#define MP_MUL_DIGIT_TESTS   5
#define MP_MUL_HIGH_TESTS    6
#define MP_DIV_QR_TESTS      10
#define MP_DIV_Q_TESTS       5
#define MP_DIV_R_TESTS       5
#define MP_DIV_DIGIT_TESTS   6
#define MP_DIV_DIGIT_Q_TESTS 4
#define MP_DIV_DIGIT_R_TESTS 4
#define MP_FDIV_DIGIT_TESTS  5
#define MP_LSHIFT_TESTS      6
#define MP_RSHIFT_TESTS      6
#define MP_COMPARE_TESTS     8
#define IS_ZERO_TESTS        4
#define IS_ONE_TESTS         4
#define EQUAL_TESTS          4
#define MP_COPY_TESTS        4
#define MP_ZERO_TESTS        3
#define CT_BITSIZE_TESTS     8

#define NUM_TESTS   28
#define TESTS_PAD   19

#define TESTS_LEN(x) (  (x) == 0  ? MP_ADD_TESTS         : \
                        (x) == 1  ? MP_ADDC_TESTS        : \
                        (x) == 2  ? MP_ADDC_DIGIT_TESTS  : \
                        (x) == 3  ? MP_ADD_DIV_TESTS     : \
                        (x) == 4  ? MP_ADDC_DIV_TESTS    : \
                        (x) == 5  ? MP_SUB_TESTS         : \
                        (x) == 6  ? MP_SUBC_TESTS        : \
                        (x) == 7  ? MP_SUB_COND_TESTS    : \
                        (x) == 8  ? MP_SUBC_COND_TESTS   : \
                        (x) == 9  ? MP_MUL_TESTS         : \
                        (x) == 10 ? MP_MUL_DIGIT_TESTS   : \
                        (x) == 11 ? MP_MUL_HIGH_TESTS    : \
                        (x) == 12 ? MP_DIV_QR_TESTS      : \
                        (x) == 13 ? MP_DIV_Q_TESTS       : \
                        (x) == 14 ? MP_DIV_R_TESTS       : \
                        (x) == 15 ? MP_DIV_DIGIT_TESTS   : \
                        (x) == 16 ? MP_DIV_DIGIT_Q_TESTS : \
                        (x) == 17 ? MP_DIV_DIGIT_R_TESTS : \
                        (x) == 18 ? MP_FDIV_DIGIT_TESTS  : \
                        (x) == 19 ? MP_LSHIFT_TESTS      : \
                        (x) == 20 ? MP_RSHIFT_TESTS      : \
                        (x) == 21 ? MP_COMPARE_TESTS     : \
                        (x) == 22 ? IS_ZERO_TESTS        : \
                        (x) == 23 ? IS_ONE_TESTS         : \
                        (x) == 24 ? EQUAL_TESTS          : \
                        (x) == 25 ? MP_COPY_TESTS        : \
                        (x) == 26 ? MP_ZERO_TESTS        : \
                        (x) == 27 ? CT_BITSIZE_TESTS     : -1)

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

// Test mp_add: multi-precision addition
void test_mp_add() {
    const int fn_idx = 0;
    
    // Test 1: Simple 2-word addition
    std::array<digit_t, 2> a1 = {5, 10};
    std::array<digit_t, 2> b1 = {3, 7};
    std::array<digit_t, 2> r1;
    std::array<digit_t, 2> expected1 = {8, 17};
    mp_add(r1, a1, b1);
    TEST(fn_idx, 0, "mp_add: [5,10] + [3,7]", ct_equal(r1, expected1));
    
    // Test 2: Addition with carry
    std::array<digit_t, 2> a2 = {MAX_DIGIT, 5};
    std::array<digit_t, 2> b2 = {1, 0};
    std::array<digit_t, 2> r2;
    std::array<digit_t, 2> expected2 = {0, 6};
    mp_add(r2, a2, b2);
    TEST(fn_idx, 1, "mp_add: carry propagation", ct_equal(r2, expected2));
    
    // Test 3: Zero addition
    std::array<digit_t, 3> a3 = {1, 2, 3};
    std::array<digit_t, 3> b3 = {0, 0, 0};
    std::array<digit_t, 3> r3;
    mp_add(r3, a3, b3);
    TEST(fn_idx, 2, "mp_add: add zero", ct_equal(r3, a3));
    
    // Test 4: Different sizes - truncation
    std::array<digit_t, 3> a4 = {100, 200, 300};
    std::array<digit_t, 2> b4 = {50, 60};
    std::array<digit_t, 2> r4;
    std::array<digit_t, 2> expected4 = {150, 260};
    mp_add(r4, a4, b4);
    TEST(fn_idx, 3, "mp_add: different sizes", ct_equal(r4, expected4));
    
    // Test 5: All max digits
    std::array<digit_t, 2> a5 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> b5 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> r5;
    std::array<digit_t, 2> expected5 = {MAX_DIGIT - 1, MAX_DIGIT};
    mp_add(r5, a5, b5);
    TEST(fn_idx, 4, "mp_add: max values overflow", ct_equal(r5, expected5));
    
    // Test 6: Single word
    std::array<digit_t, 1> a6 = {42};
    std::array<digit_t, 1> b6 = {58};
    std::array<digit_t, 1> r6;
    std::array<digit_t, 1> expected6 = {100};
    mp_add(r6, a6, b6);
    TEST(fn_idx, 5, "mp_add: single word", ct_equal(r6, expected6));
    
    // Test 7: Multiple carries
    std::array<digit_t, 4> a7 = {MAX_DIGIT, MAX_DIGIT, MAX_DIGIT, 0};
    std::array<digit_t, 4> b7 = {1, 0, 0, 0};
    std::array<digit_t, 4> r7;
    std::array<digit_t, 4> expected7 = {0, 0, 0, 1};
    mp_add(r7, a7, b7);
    TEST(fn_idx, 6, "mp_add: multiple carries", ct_equal(r7, expected7));
    
    // Test 8: Large numbers
    std::array<digit_t, 4> a8 = {0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL, 0x1111111111111111ULL, 0x2222222222222222ULL};
    std::array<digit_t, 4> b8 = {0x0FEDCBA987654321ULL, 0x0123456789ABCDEFULL, 0x3333333333333333ULL, 0x4444444444444444ULL};
    std::array<digit_t, 4> r8;
    std::array<digit_t, 4> expected8 = {0x2222222222222211ULL, 0xFFFFFFFFFFFFFFFFULL, 0x4444444444444444ULL, 0x6666666666666666ULL};
    mp_add(r8, a8, b8);
    TEST(fn_idx, 7, "mp_add: large numbers", ct_equal(r8, expected8));
}

// Test mp_addc: multi-precision addition with carry
void test_mp_addc() {
    const int fn_idx = 1;
    SKIP_IF_FAILED(0);  // Depends on mp_add
    
    // Test 1: Simple addition with carry = 0
    std::array<digit_t, 2> a1 = {5, 10};
    std::array<digit_t, 2> b1 = {3, 7};
    std::array<digit_t, 2> r1;
    digit_t carry1 = 0;
    mp_addc(r1, carry1, a1, b1);
    std::array<digit_t, 2> expected1 = {8, 17};
    TEST(fn_idx, 0, "mp_addc: carry=0", ct_equal(r1, expected1) && carry1 == 0);
    
    // Test 2: Addition with carry = 1
    std::array<digit_t, 2> a2 = {5, 10};
    std::array<digit_t, 2> b2 = {3, 7};
    std::array<digit_t, 2> r2;
    digit_t carry2 = 1;
    mp_addc(r2, carry2, a2, b2);
    std::array<digit_t, 2> expected2 = {9, 17};
    TEST(fn_idx, 1, "mp_addc: carry=1", ct_equal(r2, expected2) && carry2 == 0);
    
    // Test 3: Carry propagation
    std::array<digit_t, 2> a3 = {MAX_DIGIT, 5};
    std::array<digit_t, 2> b3 = {0, 0};
    std::array<digit_t, 2> r3;
    digit_t carry3 = 1;
    mp_addc(r3, carry3, a3, b3);
    std::array<digit_t, 2> expected3 = {0, 6};
    TEST(fn_idx, 2, "mp_addc: carry propagation", ct_equal(r3, expected3) && carry3 == 0);
    
    // Test 4: Final carry out
    std::array<digit_t, 2> a4 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> b4 = {1, 0};
    std::array<digit_t, 2> r4;
    digit_t carry4 = 0;
    mp_addc(r4, carry4, a4, b4);
    std::array<digit_t, 2> expected4 = {0, 0};
    TEST(fn_idx, 3, "mp_addc: final carry", ct_equal(r4, expected4) && carry4 == 1);
    
    // Test 5: All max with carry
    std::array<digit_t, 2> a5 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> b5 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> r5;
    digit_t carry5 = 1;
    mp_addc(r5, carry5, a5, b5);
    std::array<digit_t, 2> expected5 = {MAX_DIGIT, MAX_DIGIT};
    TEST(fn_idx, 4, "mp_addc: max overflow with carry", ct_equal(r5, expected5) && carry5 == 1);
    
    // Test 6: Zero with carry
    std::array<digit_t, 2> a6 = {0, 0};
    std::array<digit_t, 2> b6 = {0, 0};
    std::array<digit_t, 2> r6;
    digit_t carry6 = 1;
    mp_addc(r6, carry6, a6, b6);
    std::array<digit_t, 2> expected6 = {1, 0};
    TEST(fn_idx, 5, "mp_addc: zero with carry", ct_equal(r6, expected6) && carry6 == 0);
}

// Test mp_addc_digit: multi-precision addition with single digit and carry
void test_mp_addc_digit() {
    const int fn_idx = 2;
    SKIP_IF_FAILED(0);  // Depends on mp_add
    
    // Test 1: Simple addition with digit, carry = 0
    std::array<digit_t, 2> a1 = {5, 10};
    digit_t b1 = 7;
    std::array<digit_t, 2> r1;
    digit_t carry1 = 0;
    mp_addc(r1, carry1, a1, b1);
    std::array<digit_t, 2> expected1 = {12, 10};
    TEST(fn_idx, 0, "mp_addc_digit: carry=0", ct_equal(r1, expected1) && carry1 == 0);
    
    // Test 2: Addition with carry = 1
    std::array<digit_t, 2> a2 = {5, 10};
    digit_t b2 = 7;
    std::array<digit_t, 2> r2;
    digit_t carry2 = 1;
    mp_addc(r2, carry2, a2, b2);
    std::array<digit_t, 2> expected2 = {13, 10};
    TEST(fn_idx, 1, "mp_addc_digit: carry=1", ct_equal(r2, expected2) && carry2 == 0);
    
    // Test 3: Overflow to next word
    std::array<digit_t, 2> a3 = {MAX_DIGIT, 5};
    digit_t b3 = 1;
    std::array<digit_t, 2> r3;
    digit_t carry3 = 0;
    mp_addc(r3, carry3, a3, b3);
    std::array<digit_t, 2> expected3 = {0, 6};
    TEST(fn_idx, 2, "mp_addc_digit: overflow", ct_equal(r3, expected3) && carry3 == 0);
    
    // Test 4: Final carry out
    std::array<digit_t, 1> a4 = {MAX_DIGIT};
    digit_t b4 = 1;
    std::array<digit_t, 1> r4;
    digit_t carry4 = 0;
    mp_addc(r4, carry4, a4, b4);
    std::array<digit_t, 1> expected4 = {0};
    TEST(fn_idx, 3, "mp_addc_digit: final carry", ct_equal(r4, expected4) && carry4 == 1);
    
    // Test 5: Carry propagation
    std::array<digit_t, 3> a5 = {MAX_DIGIT, MAX_DIGIT, 5};
    digit_t b5 = 1;
    std::array<digit_t, 3> r5;
    digit_t carry5 = 0;
    mp_addc(r5, carry5, a5, b5);
    std::array<digit_t, 3> expected5 = {0, 0, 6};
    TEST(fn_idx, 4, "mp_addc_digit: carry propagation", ct_equal(r5, expected5) && carry5 == 0);
}

// Test mp_add_and_divide: addition followed by right shift by one word
void test_mp_add_and_divide() {
    const int fn_idx = 3;
    SKIP_IF_FAILED(0);  // Depends on mp_add
    
    // Test 1: Even sum
    std::array<digit_t, 2> a1 = {10, 20};
    std::array<digit_t, 2> b1 = {MAX_DIGIT - 9, 8};
    std::array<digit_t, 2> r1;
    mp_add_and_divide(r1, a1, b1);
    std::array<digit_t, 2> expected1 = {29, 0};
    TEST(fn_idx, 0, "mp_add_and_divide: even sum", ct_equal(r1, expected1));
    
    // Test 2: Odd sum (loses LSB)
    std::array<digit_t, 2> a2 = {10, 20};
    std::array<digit_t, 2> b2 = {7, 9};
    std::array<digit_t, 2> r2;
    mp_add_and_divide(r2, a2, b2);
    std::array<digit_t, 2> expected2 = {29, 0};
    TEST(fn_idx, 1, "mp_add_and_divide: odd sum", ct_equal(r2, expected2));
    
    // Test 3: With carry into division
    std::array<digit_t, 2> a3 = {MAX_DIGIT, 0};
    std::array<digit_t, 2> b3 = {1, 0};
    std::array<digit_t, 2> r3;
    mp_add_and_divide(r3, a3, b3);
    std::array<digit_t, 2> expected3 = {1, 0};
    TEST(fn_idx, 2, "mp_add_and_divide: carry", ct_equal(r3, expected3));
    
    // Test 4: Zero
    std::array<digit_t, 2> a4 = {0, 0};
    std::array<digit_t, 2> b4 = {0, 0};
    std::array<digit_t, 2> r4;
    mp_add_and_divide(r4, a4, b4);
    std::array<digit_t, 2> expected4 = {0, 0};
    TEST(fn_idx, 3, "mp_add_and_divide: zero", ct_equal(r4, expected4));
}

// Test mp_addc_and_divide: addition with carry followed by right shift by one word
void test_mp_addc_and_divide() {
    const int fn_idx = 4;
    SKIP_IF_FAILED(1);  // Depends on mp_addc
    
    // Test 1: Even sum with carry = 0
    std::array<digit_t, 2> a1 = {10, 20};
    std::array<digit_t, 2> b1 = {6, 8};
    std::array<digit_t, 2> r1;
    digit_t carry1 = 0;
    mp_addc_and_divide(r1, carry1, a1, b1);
    std::array<digit_t, 2> expected1 = {28, 0};
    TEST(fn_idx, 0, "mp_addc_and_divide: carry=0", ct_equal(r1, expected1) && carry1 == 0);
    
    // Test 2: With input carry = 1
    std::array<digit_t, 2> a2 = {10, 20};
    std::array<digit_t, 2> b2 = {MAX_DIGIT - 10, 8};
    std::array<digit_t, 2> r2;
    digit_t carry2 = 1;
    mp_addc_and_divide(r2, carry2, a2, b2);
    std::array<digit_t, 2> expected2 = {29, 0};
    TEST(fn_idx, 1, "mp_addc_and_divide: carry=1", ct_equal(r2, expected2) && carry2 == 0);
    
    // Test 3: Overflow carry
    std::array<digit_t, 2> a3 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> b3 = {0, 0};
    std::array<digit_t, 2> r3;
    digit_t carry3 = 1;
    mp_addc_and_divide(r3, carry3, a3, b3);
    std::array<digit_t, 2> expected3 = {0, 1};
    TEST(fn_idx, 2, "mp_addc_and_divide: overflow", ct_equal(r3, expected3) && carry3 == 0);
    
    // Test 4: All max with carry
    std::array<digit_t, 2> a4 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> b4 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> r4;
    digit_t carry4 = 1;
    mp_addc_and_divide(r4, carry4, a4, b4);
    std::array<digit_t, 2> expected4 = {MAX_DIGIT, 1};
    TEST(fn_idx, 3, "mp_addc_and_divide: all max", ct_equal(r4, expected4) && carry4 == 0);
}

// Test mp_sub: multi-precision subtraction
void test_mp_sub() {
    const int fn_idx = 5;
    
    // Test 1: Simple subtraction
    std::array<digit_t, 2> a1 = {10, 20};
    std::array<digit_t, 2> b1 = {3, 5};
    std::array<digit_t, 2> r1;
    std::array<digit_t, 2> expected1 = {7, 15};
    mp_sub(r1, a1, b1);
    TEST(fn_idx, 0, "mp_sub: [10,20] - [3,5]", ct_equal(r1, expected1));
    
    // Test 2: Subtraction with borrow
    std::array<digit_t, 2> a2 = {0, 10};
    std::array<digit_t, 2> b2 = {1, 0};
    std::array<digit_t, 2> r2;
    std::array<digit_t, 2> expected2 = {MAX_DIGIT, 9};
    mp_sub(r2, a2, b2);
    TEST(fn_idx, 1, "mp_sub: borrow propagation", ct_equal(r2, expected2));
    
    // Test 3: Subtract zero
    std::array<digit_t, 3> a3 = {5, 10, 15};
    std::array<digit_t, 3> b3 = {0, 0, 0};
    std::array<digit_t, 3> r3;
    mp_sub(r3, a3, b3);
    TEST(fn_idx, 2, "mp_sub: subtract zero", ct_equal(r3, a3));
    
    // Test 4: Subtract from self
    std::array<digit_t, 2> a4 = {100, 200};
    std::array<digit_t, 2> b4 = {100, 200};
    std::array<digit_t, 2> r4;
    std::array<digit_t, 2> expected4 = {0, 0};
    mp_sub(r4, a4, b4);
    TEST(fn_idx, 3, "mp_sub: a - a = 0", ct_equal(r4, expected4));
    
    // Test 5: Underflow (wraps around)
    std::array<digit_t, 2> a5 = {0, 0};
    std::array<digit_t, 2> b5 = {1, 0};
    std::array<digit_t, 2> r5;
    std::array<digit_t, 2> expected5 = {MAX_DIGIT, MAX_DIGIT};
    mp_sub(r5, a5, b5);
    TEST(fn_idx, 4, "mp_sub: underflow wrap", ct_equal(r5, expected5));
    
    // Test 6: Single word
    std::array<digit_t, 1> a6 = {100};
    std::array<digit_t, 1> b6 = {42};
    std::array<digit_t, 1> r6;
    std::array<digit_t, 1> expected6 = {58};
    mp_sub(r6, a6, b6);
    TEST(fn_idx, 5, "mp_sub: single word", ct_equal(r6, expected6));
    
    // Test 7: Multiple borrows
    std::array<digit_t, 4> a7 = {0, 0, 0, 1};
    std::array<digit_t, 4> b7 = {1, 0, 0, 0};
    std::array<digit_t, 4> r7;
    std::array<digit_t, 4> expected7 = {MAX_DIGIT, MAX_DIGIT, MAX_DIGIT, 0};
    mp_sub(r7, a7, b7);
    TEST(fn_idx, 6, "mp_sub: multiple borrows", ct_equal(r7, expected7));
    
    // Test 8: Large numbers
    std::array<digit_t, 4> a8 = {0xFEDCBA9876543210ULL, 0x0123456789ABCDEFULL, 0x3333333333333333ULL, 0x4444444444444444ULL};
    std::array<digit_t, 4> b8 = {0x123456789ABCDEF0ULL, 0x0FEDCBA987654321ULL, 0x1111111111111111ULL, 0x2222222222222222ULL};
    std::array<digit_t, 4> r8;
    std::array<digit_t, 4> expected8 = {0xECA8641FDB975320ULL, 0xF13579BE02468ACEULL, 0x2222222222222221ULL, 0x2222222222222222ULL};
    mp_sub(r8, a8, b8);
    TEST(fn_idx, 7, "mp_sub: large numbers", ct_equal(r8, expected8));
}

// Test mp_subc: multi-precision subtraction with borrow
void test_mp_subc() {
    const int fn_idx = 6;
    SKIP_IF_FAILED(5);  // Depends on mp_sub
    
    // Test 1: Simple subtraction with borrow = 0
    std::array<digit_t, 2> a1 = {10, 20};
    std::array<digit_t, 2> b1 = {3, 5};
    std::array<digit_t, 2> r1;
    digit_t borrow1 = 0;
    mp_subc(r1, borrow1, a1, b1);
    std::array<digit_t, 2> expected1 = {7, 15};
    TEST(fn_idx, 0, "mp_subc: borrow=0", ct_equal(r1, expected1) && borrow1 == 0);
    
    // Test 2: Subtraction with borrow = 1
    std::array<digit_t, 2> a2 = {10, 20};
    std::array<digit_t, 2> b2 = {3, 5};
    std::array<digit_t, 2> r2;
    digit_t borrow2 = 1;
    mp_subc(r2, borrow2, a2, b2);
    std::array<digit_t, 2> expected2 = {6, 15};
    TEST(fn_idx, 1, "mp_subc: borrow=1", ct_equal(r2, expected2) && borrow2 == 0);
    
    // Test 3: Borrow propagation
    std::array<digit_t, 2> a3 = {0, 10};
    std::array<digit_t, 2> b3 = {0, 0};
    std::array<digit_t, 2> r3;
    digit_t borrow3 = 1;
    mp_subc(r3, borrow3, a3, b3);
    std::array<digit_t, 2> expected3 = {MAX_DIGIT, 9};
    TEST(fn_idx, 2, "mp_subc: borrow propagation", ct_equal(r3, expected3) && borrow3 == 0);
    
    // Test 4: Final borrow out
    std::array<digit_t, 2> a4 = {0, 0};
    std::array<digit_t, 2> b4 = {1, 0};
    std::array<digit_t, 2> r4;
    digit_t borrow4 = 0;
    mp_subc(r4, borrow4, a4, b4);
    std::array<digit_t, 2> expected4 = {MAX_DIGIT, MAX_DIGIT};
    TEST(fn_idx, 3, "mp_subc: final borrow", ct_equal(r4, expected4) && borrow4 == 1);
    
    // Test 5: Zero minus borrow
    std::array<digit_t, 2> a5 = {100, 200};
    std::array<digit_t, 2> b5 = {100, 200};
    std::array<digit_t, 2> r5;
    digit_t borrow5 = 1;
    mp_subc(r5, borrow5, a5, b5);
    std::array<digit_t, 2> expected5 = {MAX_DIGIT, MAX_DIGIT};
    TEST(fn_idx, 4, "mp_subc: a - a - 1", ct_equal(r5, expected5) && borrow5 == 1);
    
    // Test 6: No borrow needed
    std::array<digit_t, 2> a6 = {100, 200};
    std::array<digit_t, 2> b6 = {50, 100};
    std::array<digit_t, 2> r6;
    digit_t borrow6 = 0;
    mp_subc(r6, borrow6, a6, b6);
    std::array<digit_t, 2> expected6 = {50, 100};
    TEST(fn_idx, 5, "mp_subc: no borrow", ct_equal(r6, expected6) && borrow6 == 0);
}

// Test mp_sub_conditional: conditional multi-precision subtraction (subtracts, adds back if underflow)
void test_mp_sub_conditional() {
    const int fn_idx = 7;
    SKIP_IF_FAILED(5);  // Depends on mp_sub
    
    // Test 1: No underflow (a >= b)
    std::array<digit_t, 2> a1 = {10, 20};
    std::array<digit_t, 2> b1 = {3, 5};
    std::array<digit_t, 2> r1;
    mp_sub_conditional(r1, a1, b1);
    std::array<digit_t, 2> expected1 = {7, 15};
    TEST(fn_idx, 0, "mp_sub_conditional: a >= b subtracts", ct_equal(r1, expected1));
    
    // Test 2: With underflow (a < b) - should add back
    std::array<digit_t, 2> a2 = {5, 10};
    std::array<digit_t, 2> b2 = {10, 20};
    std::array<digit_t, 2> r2;
    mp_sub_conditional(r2, a2, b2);
    TEST(fn_idx, 1, "mp_sub_conditional: a < b preserves", ct_equal(r2, a2));
    
    // Test 3: Equal values
    std::array<digit_t, 2> a3 = {100, 200};
    std::array<digit_t, 2> b3 = {100, 200};
    std::array<digit_t, 2> r3;
    mp_sub_conditional(r3, a3, b3);
    std::array<digit_t, 2> expected3 = {0, 0};
    TEST(fn_idx, 2, "mp_sub_conditional: a == b gives zero", ct_equal(r3, expected3));
    
    // Test 4: Underflow in multi-word
    std::array<digit_t, 3> a4 = {5, 0, 1};
    std::array<digit_t, 3> b4 = {10, 0, 2};
    std::array<digit_t, 3> r4;
    mp_sub_conditional(r4, a4, b4);
    TEST(fn_idx, 3, "mp_sub_conditional: multi-word underflow", ct_equal(r4, a4));
    
    // Test 5: Close values no underflow
    std::array<digit_t, 2> a5 = {0, 10};
    std::array<digit_t, 2> b5 = {1, 0};
    std::array<digit_t, 2> r5;
    mp_sub_conditional(r5, a5, b5);
    std::array<digit_t, 2> expected5 = {MAX_DIGIT, 9};
    TEST(fn_idx, 4, "mp_sub_conditional: borrow but no underflow", ct_equal(r5, expected5));
}

// Test mp_mul: multi-precision multiplication
void test_mp_mul() {
    const int fn_idx = 9;
    
    // Test 1: Simple 1x1 multiplication
    std::array<digit_t, 1> a1 = {5};
    std::array<digit_t, 1> b1 = {7};
    std::array<digit_t, 2> r1;
    std::array<digit_t, 2> expected1 = {35, 0};
    mp_mul(r1, a1, b1);
    TEST(fn_idx, 0, "mp_mul: 5 * 7 = 35", ct_equal(r1, expected1));
    
    // Test 2: Multiplication by zero
    std::array<digit_t, 2> a2 = {123, 456};
    std::array<digit_t, 2> b2 = {0, 0};
    std::array<digit_t, 4> r2;
    std::array<digit_t, 4> expected2 = {0, 0, 0, 0};
    mp_mul(r2, a2, b2);
    TEST(fn_idx, 1, "mp_mul: multiply by zero", ct_equal(r2, expected2));
    
    // Test 3: Multiplication by one
    std::array<digit_t, 2> a3 = {123, 456};
    std::array<digit_t, 1> b3 = {1};
    std::array<digit_t, 3> r3;
    std::array<digit_t, 3> expected3 = {123, 456, 0};
    mp_mul(r3, a3, b3);
    TEST(fn_idx, 2, "mp_mul: multiply by one", ct_equal(r3, expected3));
    
    // Test 4: 2x2 multiplication
    std::array<digit_t, 2> a4 = {3, 0};
    std::array<digit_t, 2> b4 = {5, 0};
    std::array<digit_t, 4> r4;
    std::array<digit_t, 4> expected4 = {15, 0, 0, 0};
    mp_mul(r4, a4, b4);
    TEST(fn_idx, 3, "mp_mul: 2x2 low words", ct_equal(r4, expected4));
    
    // Test 5: Overflow to high word
    std::array<digit_t, 1> a5 = {MAX_DIGIT};
    std::array<digit_t, 1> b5 = {2};
    std::array<digit_t, 2> r5;
    std::array<digit_t, 2> expected5 = {MAX_DIGIT - 1, 1};
    mp_mul(r5, a5, b5);
    TEST(fn_idx, 4, "mp_mul: overflow to high", ct_equal(r5, expected5));
    
    // Test 6: MAX * MAX
    std::array<digit_t, 1> a6 = {MAX_DIGIT};
    std::array<digit_t, 1> b6 = {MAX_DIGIT};
    std::array<digit_t, 2> r6;
    std::array<digit_t, 2> expected6 = {1, MAX_DIGIT - 1};
    mp_mul(r6, a6, b6);
    TEST(fn_idx, 5, "mp_mul: MAX * MAX", ct_equal(r6, expected6));
    
    // Test 7: Truncated output
    std::array<digit_t, 2> a7 = {100, 200};
    std::array<digit_t, 2> b7 = {2, 0};
    std::array<digit_t, 2> r7;  // Truncated to 2 words
    std::array<digit_t, 2> expected7 = {200, 400};
    mp_mul(r7, a7, b7);
    TEST(fn_idx, 6, "mp_mul: truncated output", ct_equal(r7, expected7));
    
    // Test 8: Small known values
    std::array<digit_t, 2> a8 = {0x1000000000000000ULL, 0};
    std::array<digit_t, 2> b8 = {0x10, 0};
    std::array<digit_t, 4> r8;
    std::array<digit_t, 4> expected8 = {0, 1, 0, 0};
    mp_mul(r8, a8, b8);
    TEST(fn_idx, 7, "mp_mul: known overflow pattern", ct_equal(r8, expected8));
}

// Test mp_mul_digit: multiplication by single digit
void test_mp_mul_digit() {
    const int fn_idx = 10;
    SKIP_IF_FAILED(9);  // Depends on mp_mul
    
    // Test 1: Simple multiplication
    std::array<digit_t, 2> a1 = {5, 10};
    digit_t b1 = 3;
    std::array<digit_t, 3> r1;
    std::array<digit_t, 3> expected1 = {15, 30, 0};
    mp_mul_digit(r1, a1, b1);
    TEST(fn_idx, 0, "mp_mul_digit: [5,10] * 3", ct_equal(r1, expected1));
    
    // Test 2: Multiply by zero
    std::array<digit_t, 2> a2 = {123, 456};
    digit_t b2 = 0;
    std::array<digit_t, 3> r2;
    std::array<digit_t, 3> expected2 = {0, 0, 0};
    mp_mul_digit(r2, a2, b2);
    TEST(fn_idx, 1, "mp_mul_digit: multiply by 0", ct_equal(r2, expected2));
    
    // Test 3: Multiply by one
    std::array<digit_t, 2> a3 = {123, 456};
    digit_t b3 = 1;
    std::array<digit_t, 3> r3;
    std::array<digit_t, 3> expected3 = {123, 456, 0};
    mp_mul_digit(r3, a3, b3);
    TEST(fn_idx, 2, "mp_mul_digit: multiply by 1", ct_equal(r3, expected3));
    
    // Test 4: Overflow to next word
    std::array<digit_t, 1> a4 = {MAX_DIGIT};
    digit_t b4 = 2;
    std::array<digit_t, 2> r4;
    std::array<digit_t, 2> expected4 = {MAX_DIGIT - 1, 1};
    mp_mul_digit(r4, a4, b4);
    TEST(fn_idx, 3, "mp_mul_digit: overflow", ct_equal(r4, expected4));
    
    // Test 5: Large digit
    std::array<digit_t, 2> a5 = {0x1000000000000000ULL, 0};
    digit_t b5 = 0x10;
    std::array<digit_t, 3> r5;
    std::array<digit_t, 3> expected5 = {0, 1, 0};
    mp_mul_digit(r5, a5, b5);
    TEST(fn_idx, 4, "mp_mul_digit: large values", ct_equal(r5, expected5));
}

// Test mp_mul_high: multiplication keeping only high words
void test_mp_mul_high() {
    const int fn_idx = 11;
    SKIP_IF_FAILED(9);  // Depends on mp_mul
    
    // Test 1: Simple multiplication, check high words
    std::array<digit_t, 1> a1 = {MAX_DIGIT};
    std::array<digit_t, 1> b1 = {MAX_DIGIT};
    std::array<digit_t, 1> r1;
    std::array<digit_t, 1> expected1 = {MAX_DIGIT - 1};  // High word of MAX * MAX
    mp_mul_high(r1, a1, b1);
    TEST(fn_idx, 0, "mp_mul_high: MAX * MAX high word", ct_equal(r1, expected1));
    
    // Test 2: No overflow to high
    std::array<digit_t, 1> a2 = {5};
    std::array<digit_t, 1> b2 = {7};
    std::array<digit_t, 1> r2;
    std::array<digit_t, 1> expected2 = {0};
    mp_mul_high(r2, a2, b2);
    TEST(fn_idx, 1, "mp_mul_high: small values", ct_equal(r2, expected2));
    
    // Test 3: 2x2 multiplication, high words only
    std::array<digit_t, 2> a3 = {MAX_DIGIT, 1};
    std::array<digit_t, 2> b3 = {MAX_DIGIT, 1};
    std::array<digit_t, 2> r3;
    mp_mul_high(r3, a3, b3);
    // (2^64)^2 = 2^128, represented as high 2 words
    std::array<digit_t, 2> expected3 = {3, 0};  // Calculated from full multiplication
    TEST(fn_idx, 2, "mp_mul_high: 2x2 high words", ct_equal(r3, expected3));
    
    // Test 4: Multiply by zero
    std::array<digit_t, 2> a4 = {123, 456};
    std::array<digit_t, 2> b4 = {0, 0};
    std::array<digit_t, 2> r4;
    std::array<digit_t, 2> expected4 = {0, 0};
    mp_mul_high(r4, a4, b4);
    TEST(fn_idx, 3, "mp_mul_high: multiply by zero", ct_equal(r4, expected4));
    
    // Test 5: Large value overflow
    std::array<digit_t, 1> a5 = {0x8000000000000000ULL};
    std::array<digit_t, 1> b5 = {2};
    std::array<digit_t, 1> r5;
    std::array<digit_t, 1> expected5 = {1};
    mp_mul_high(r5, a5, b5);
    TEST(fn_idx, 4, "mp_mul_high: overflow", ct_equal(r5, expected5));
    
    // Test 6: Max digit times 2
    std::array<digit_t, 1> a6 = {MAX_DIGIT};
    std::array<digit_t, 1> b6 = {2};
    std::array<digit_t, 1> r6;
    std::array<digit_t, 1> expected6 = {1};
    mp_mul_high(r6, a6, b6);
    TEST(fn_idx, 5, "mp_mul_high: MAX * 2", ct_equal(r6, expected6));
}

// Test mp_div_qr: multi-precision division with remainder
void test_mp_div_qr() {
    const int fn_idx = 12;
    SKIP_IF_FAILED(9);  // Depends on mp_mul
    
    // Test 1: Simple division
    std::array<digit_t, 2> num1 = {100, 0};
    std::array<digit_t, 1> den1 = {10};
    std::array<digit_t, 2> quo1;
    std::array<digit_t, 1> rem1;
    std::array<digit_t, 2> expected_quo1 = {10, 0};
    std::array<digit_t, 1> expected_rem1 = {0};
    mp_div_qr(quo1, rem1, num1, den1);
    TEST(fn_idx, 0, "mp_div_qr: 100 / 10", ct_equal(quo1, expected_quo1) && ct_equal(rem1, expected_rem1));
    
    // Test 2: Division with remainder
    std::array<digit_t, 2> num2 = {107, 0};
    std::array<digit_t, 1> den2 = {10};
    std::array<digit_t, 2> quo2;
    std::array<digit_t, 1> rem2;
    std::array<digit_t, 2> expected_quo2 = {10, 0};
    std::array<digit_t, 1> expected_rem2 = {7};
    mp_div_qr(quo2, rem2, num2, den2);
    TEST(fn_idx, 1, "mp_div_qr: 107 / 10 = 10 r 7", ct_equal(quo2, expected_quo2) && ct_equal(rem2, expected_rem2));
    
    // Test 3: Divide by self
    std::array<digit_t, 2> num3 = {123, 456};
    std::array<digit_t, 2> den3 = {123, 456};
    std::array<digit_t, 1> quo3;
    std::array<digit_t, 2> rem3;
    std::array<digit_t, 1> expected_quo3 = {1};
    std::array<digit_t, 2> expected_rem3 = {0, 0};
    mp_div_qr(quo3, rem3, num3, den3);
    TEST(fn_idx, 2, "mp_div_qr: a / a = 1", ct_equal(quo3, expected_quo3) && ct_equal(rem3, expected_rem3));
    
    // Test 4: Numerator smaller than denominator
    std::array<digit_t, 2> num4 = {5, 0};
    std::array<digit_t, 1> den4 = {10};
    std::array<digit_t, 2> quo4;
    std::array<digit_t, 1> rem4;
    std::array<digit_t, 2> expected_quo4 = {0, 0};
    std::array<digit_t, 1> expected_rem4 = {5};
    mp_div_qr(quo4, rem4, num4, den4);
    TEST(fn_idx, 3, "mp_div_qr: 5 / 10 = 0 r 5", ct_equal(quo4, expected_quo4) && ct_equal(rem4, expected_rem4));
    
    // Test 5: Division by 1
    std::array<digit_t, 2> num5 = {123, 456};
    std::array<digit_t, 1> den5 = {1};
    std::array<digit_t, 2> quo5;
    std::array<digit_t, 1> rem5;
    std::array<digit_t, 2> expected_quo5 = {123, 456};
    std::array<digit_t, 1> expected_rem5 = {0};
    mp_div_qr(quo5, rem5, num5, den5);
    TEST(fn_idx, 4, "mp_div_qr: divide by 1", ct_equal(quo5, expected_quo5) && ct_equal(rem5, expected_rem5));
    
    // Test 6: Large single digit divisor
    std::array<digit_t, 2> num6 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 1> den6 = {MAX_DIGIT};
    std::array<digit_t, 2> quo6;
    std::array<digit_t, 1> rem6;
    mp_div_qr(quo6, rem6, num6, den6);
    // Verify: quo * den + rem == num
    std::array<digit_t, 2> verify6;
    mp_mul(verify6, quo6, den6);
    verify6[0] += rem6[0];
    if (verify6[0] < rem6[0]) verify6[1]++;
    TEST(fn_idx, 5, "mp_div_qr: large divisor verify", ct_equal(verify6, num6));
    
    // Test 7: Power of 2 division
    std::array<digit_t, 2> num7 = {0, 1};  // 2^64
    std::array<digit_t, 1> den7 = {2};
    std::array<digit_t, 2> quo7;
    std::array<digit_t, 1> rem7;
    std::array<digit_t, 2> expected_quo7 = {0x8000000000000000ULL, 0};
    std::array<digit_t, 1> expected_rem7 = {0};
    mp_div_qr(quo7, rem7, num7, den7);
    TEST(fn_idx, 6, "mp_div_qr: power of 2", ct_equal(quo7, expected_quo7) && ct_equal(rem7, expected_rem7));
    
    // Test 8: Multi-word division
    std::array<digit_t, 4> num8 = {100, 200, 300, 400};
    std::array<digit_t, 1> den8 = {10};
    std::array<digit_t, 4> quo8;
    std::array<digit_t, 1> rem8;
    mp_div_qr(quo8, rem8, num8, den8);
    // Verify: quo * den + rem == num (truncated to 4 words)
    std::array<digit_t, 4> verify8;
    mp_mul(verify8, quo8, den8);
    verify8[0] += rem8[0];
    if (verify8[0] < rem8[0]) verify8[1]++;
    TEST(fn_idx, 7, "mp_div_qr: multi-word verify", ct_equal(verify8, num8));
    
    // Test 9: 2-word by 2-word division
    std::array<digit_t, 2> num9 = {1000, 2000};
    std::array<digit_t, 2> den9 = {100, 200};
    std::array<digit_t, 1> quo9;
    std::array<digit_t, 2> rem9;
    mp_div_qr(quo9, rem9, num9, den9);
    // Verify reconstruction
    std::array<digit_t, 2> verify9;
    mp_mul(verify9, quo9, den9);
    digit_t carry9 = 0;
    for (size_t i = 0; i < 2; i++) {
        addc(verify9[i], carry9, verify9[i], rem9[i]);
    }
    TEST(fn_idx, 8, "mp_div_qr: 2x2 verify", ct_equal(verify9, num9));
    
    // Test 10: Remainder bounds check
    std::array<digit_t, 3> num10 = {0xABCDEF, 0x123456, 0x789ABC};
    std::array<digit_t, 2> den10 = {0x1000, 0x2000};
    std::array<digit_t, 2> quo10;
    std::array<digit_t, 2> rem10;
    mp_div_qr(quo10, rem10, num10, den10);
    // Verify rem < den
    bool rem_less = (rem10[1] < den10[1]) || (rem10[1] == den10[1] && rem10[0] < den10[0]);
    TEST(fn_idx, 9, "mp_div_qr: remainder < divisor", rem_less);
}

// Test mp_div_digit_qr: division by single digit
void test_mp_div_digit_qr() {
    const int fn_idx = 15;
    SKIP_IF_FAILED(12);  // Depends on mp_div_qr
    
    // Test 1: Simple division
    std::array<digit_t, 2> num1 = {100, 0};
    digit_t den1 = 10;
    std::array<digit_t, 2> quo1;
    digit_t rem1;
    std::array<digit_t, 2> expected_quo1 = {10, 0};
    mp_div_digit_qr(quo1, rem1, num1, den1);
    TEST(fn_idx, 0, "mp_div_digit_qr: 100 / 10", ct_equal(quo1, expected_quo1) && rem1 == 0);
    
    // Test 2: With remainder
    std::array<digit_t, 2> num2 = {107, 0};
    digit_t den2 = 10;
    std::array<digit_t, 2> quo2;
    digit_t rem2;
    std::array<digit_t, 2> expected_quo2 = {10, 0};
    mp_div_digit_qr(quo2, rem2, num2, den2);
    TEST(fn_idx, 1, "mp_div_digit_qr: 107 / 10 = 10 r 7", ct_equal(quo2, expected_quo2) && rem2 == 7);
    
    // Test 3: Divide by 1
    std::array<digit_t, 2> num3 = {123, 456};
    digit_t den3 = 1;
    std::array<digit_t, 2> quo3;
    digit_t rem3;
    mp_div_digit_qr(quo3, rem3, num3, den3);
    TEST(fn_idx, 2, "mp_div_digit_qr: divide by 1", ct_equal(quo3, num3) && rem3 == 0);
    
    // Test 4: Numerator smaller
    std::array<digit_t, 1> num4 = {5};
    digit_t den4 = 10;
    std::array<digit_t, 1> quo4;
    digit_t rem4;
    std::array<digit_t, 1> expected_quo4 = {0};
    mp_div_digit_qr(quo4, rem4, num4, den4);
    TEST(fn_idx, 3, "mp_div_digit_qr: 5 / 10 = 0 r 5", ct_equal(quo4, expected_quo4) && rem4 == 5);
    
    // Test 5: Large divisor
    std::array<digit_t, 2> num5 = {MAX_DIGIT, MAX_DIGIT};
    digit_t den5 = MAX_DIGIT;
    std::array<digit_t, 2> quo5;
    digit_t rem5;
    mp_div_digit_qr(quo5, rem5, num5, den5);
    // Verify: quo * den + rem == num
    std::array<digit_t, 3> verify5;
    mp_mul_digit(verify5, quo5, den5);
    mp_add(verify5, verify5, rem5);
    std::array<digit_t, 2> result5;
    copy(result5, verify5);

    TEST(fn_idx, 4, "mp_div_digit_qr: large verify", ct_equal(result5, num5) && verify5[2] == 0 && rem5 < den5);
    
    // Test 6: Multi-word
    std::array<digit_t, 4> num6 = {1234, 5678, 9012, 3456};
    digit_t den6 = 123;
    std::array<digit_t, 4> quo6;
    digit_t rem6;
    mp_div_digit_qr(quo6, rem6, num6, den6);
    // Verify
    std::array<digit_t, 5> verify6;
    mp_mul_digit(verify6, quo6, den6);
    verify6[0] += rem6;
    if (verify6[0] < rem6) verify6[1]++;
    TEST(fn_idx, 5, "mp_div_digit_qr: multi-word verify", 
         verify6[0] == num6[0] && verify6[1] == num6[1] && 
         verify6[2] == num6[2] && verify6[3] == num6[3] && rem6 < den6);
}

// Test mp_div_q: division returning only quotient
void test_mp_div_q() {
    const int fn_idx = 13;
    SKIP_IF_FAILED(12);  // Depends on mp_div_qr
    
    // Test 1: Simple division
    std::array<digit_t, 2> num1 = {100, 0};
    std::array<digit_t, 1> den1 = {10};
    std::array<digit_t, 2> quo1;
    mp_div_q(quo1, num1, den1);
    std::array<digit_t, 2> expected1 = {10, 0};
    TEST(fn_idx, 0, "mp_div_q: 100 / 10", ct_equal(quo1, expected1));
    
    // Test 2: With remainder (ignored)
    std::array<digit_t, 2> num2 = {107, 0};
    std::array<digit_t, 1> den2 = {10};
    std::array<digit_t, 2> quo2;
    mp_div_q(quo2, num2, den2);
    std::array<digit_t, 2> expected2 = {10, 0};
    TEST(fn_idx, 1, "mp_div_q: 107 / 10 = 10", ct_equal(quo2, expected2));
    
    // Test 3: Divide by self
    std::array<digit_t, 2> num3 = {123, 456};
    std::array<digit_t, 2> den3 = {123, 456};
    std::array<digit_t, 1> quo3;
    mp_div_q(quo3, num3, den3);
    std::array<digit_t, 1> expected3 = {1};
    TEST(fn_idx, 2, "mp_div_q: a / a = 1", ct_equal(quo3, expected3));
    
    // Test 4: Numerator smaller
    std::array<digit_t, 2> num4 = {5, 0};
    std::array<digit_t, 1> den4 = {10};
    std::array<digit_t, 2> quo4;
    mp_div_q(quo4, num4, den4);
    std::array<digit_t, 2> expected4 = {0, 0};
    TEST(fn_idx, 3, "mp_div_q: 5 / 10 = 0", ct_equal(quo4, expected4));
    
    // Test 5: Division by 1
    std::array<digit_t, 2> num5 = {123, 456};
    std::array<digit_t, 1> den5 = {1};
    std::array<digit_t, 2> quo5;
    mp_div_q(quo5, num5, den5);
    std::array<digit_t, 2> expected5 = {123, 456};
    TEST(fn_idx, 4, "mp_div_q: divide by 1", ct_equal(quo5, expected5));
}

// Test mp_div_r: division returning only remainder
void test_mp_div_r() {
    const int fn_idx = 14;
    SKIP_IF_FAILED(12);  // Depends on mp_div_qr
    
    // Test 1: No remainder
    std::array<digit_t, 2> num1 = {100, 0};
    std::array<digit_t, 1> den1 = {10};
    std::array<digit_t, 1> rem1;
    mp_div_r(rem1, num1, den1);
    std::array<digit_t, 1> expected1 = {0};
    TEST(fn_idx, 0, "mp_div_r: 100 % 10 = 0", ct_equal(rem1, expected1));
    
    // Test 2: With remainder
    std::array<digit_t, 2> num2 = {107, 0};
    std::array<digit_t, 1> den2 = {10};
    std::array<digit_t, 1> rem2;
    mp_div_r(rem2, num2, den2);
    std::array<digit_t, 1> expected2 = {7};
    TEST(fn_idx, 1, "mp_div_r: 107 % 10 = 7", ct_equal(rem2, expected2));
    
    // Test 3: Divide by self (remainder = 0)
    std::array<digit_t, 2> num3 = {123, 456};
    std::array<digit_t, 2> den3 = {123, 456};
    std::array<digit_t, 2> rem3;
    mp_div_r(rem3, num3, den3);
    std::array<digit_t, 2> expected3 = {0, 0};
    TEST(fn_idx, 2, "mp_div_r: a % a = 0", ct_equal(rem3, expected3));
    
    // Test 4: Numerator smaller (remainder = numerator)
    std::array<digit_t, 2> num4 = {5, 0};
    std::array<digit_t, 1> den4 = {10};
    std::array<digit_t, 1> rem4;
    mp_div_r(rem4, num4, den4);
    std::array<digit_t, 1> expected4 = {5};
    TEST(fn_idx, 3, "mp_div_r: 5 % 10 = 5", ct_equal(rem4, expected4));
    
    // Test 5: Division by 1 (remainder = 0)
    std::array<digit_t, 2> num5 = {123, 456};
    std::array<digit_t, 1> den5 = {1};
    std::array<digit_t, 1> rem5;
    mp_div_r(rem5, num5, den5);
    std::array<digit_t, 1> expected5 = {0};
    TEST(fn_idx, 4, "mp_div_r: x % 1 = 0", ct_equal(rem5, expected5));
}

// Test mp_div_digit_q: division by single digit returning only quotient
void test_mp_div_digit_q() {
    const int fn_idx = 16;
    SKIP_IF_FAILED(15);  // Depends on mp_div_digit_qr
    
    // Test 1: Simple division
    std::array<digit_t, 2> num1 = {100, 0};
    digit_t den1 = 10;
    std::array<digit_t, 2> quo1;
    mp_div_digit_q(quo1, num1, den1);
    std::array<digit_t, 2> expected1 = {10, 0};
    TEST(fn_idx, 0, "mp_div_digit_q: 100 / 10", ct_equal(quo1, expected1));
    
    // Test 2: With remainder (ignored)
    std::array<digit_t, 2> num2 = {107, 0};
    digit_t den2 = 10;
    std::array<digit_t, 2> quo2;
    mp_div_digit_q(quo2, num2, den2);
    std::array<digit_t, 2> expected2 = {10, 0};
    TEST(fn_idx, 1, "mp_div_digit_q: 107 / 10 = 10", ct_equal(quo2, expected2));
    
    // Test 3: Divide by 1
    std::array<digit_t, 2> num3 = {123, 456};
    digit_t den3 = 1;
    std::array<digit_t, 2> quo3;
    mp_div_digit_q(quo3, num3, den3);
    TEST(fn_idx, 2, "mp_div_digit_q: divide by 1", ct_equal(quo3, num3));
    
    // Test 4: Numerator smaller
    std::array<digit_t, 1> num4 = {5};
    digit_t den4 = 10;
    std::array<digit_t, 1> quo4;
    mp_div_digit_q(quo4, num4, den4);
    std::array<digit_t, 1> expected4 = {0};
    TEST(fn_idx, 3, "mp_div_digit_q: 5 / 10 = 0", ct_equal(quo4, expected4));
}

// Test mp_div_digit_r: division by single digit returning only remainder
void test_mp_div_digit_r() {
    const int fn_idx = 17;
    SKIP_IF_FAILED(15);  // Depends on mp_div_digit_qr
    
    // Test 1: No remainder
    std::array<digit_t, 2> num1 = {100, 0};
    digit_t den1 = 10;
    digit_t rem1;
    mp_div_digit_r(rem1, num1, den1);
    TEST(fn_idx, 0, "mp_div_digit_r: 100 % 10 = 0", rem1 == 0);
    
    // Test 2: With remainder
    std::array<digit_t, 2> num2 = {107, 0};
    digit_t den2 = 10;
    digit_t rem2;
    mp_div_digit_r(rem2, num2, den2);
    TEST(fn_idx, 1, "mp_div_digit_r: 107 % 10 = 7", rem2 == 7);
    
    // Test 3: Divide by 1
    std::array<digit_t, 2> num3 = {123, 456};
    digit_t den3 = 1;
    digit_t rem3;
    mp_div_digit_r(rem3, num3, den3);
    TEST(fn_idx, 2, "mp_div_digit_r: x % 1 = 0", rem3 == 0);
    
    // Test 4: Numerator smaller
    std::array<digit_t, 1> num4 = {5};
    digit_t den4 = 10;
    digit_t rem4;
    mp_div_digit_r(rem4, num4, den4);
    TEST(fn_idx, 3, "mp_div_digit_r: 5 % 10 = 5", rem4 == 5);
}

// Test mp_fdiv_digit_qr: fast division by single digit
void test_mp_fdiv_digit_qr() {
    const int fn_idx = 18;
    SKIP_IF_FAILED(15);  // Depends on mp_div_digit_qr
    
    // Test 1: Simple division
    std::array<digit_t, 2> num1 = {100, 0};
    digit_t den1 = 10;
    std::array<digit_t, 2> quo1;
    digit_t rem1;
    mp_fdiv_digit_qr(quo1, rem1, num1, den1);
    std::array<digit_t, 2> expected_quo1 = {10, 0};
    TEST(fn_idx, 0, "mp_fdiv_digit_qr: 100 / 10", ct_equal(quo1, expected_quo1) && rem1 == 0);
    
    // Test 2: With remainder
    std::array<digit_t, 2> num2 = {107, 0};
    digit_t den2 = 10;
    std::array<digit_t, 2> quo2;
    digit_t rem2;
    mp_fdiv_digit_qr(quo2, rem2, num2, den2);
    std::array<digit_t, 2> expected_quo2 = {10, 0};
    TEST(fn_idx, 1, "mp_fdiv_digit_qr: 107 / 10 = 10 r 7", ct_equal(quo2, expected_quo2) && rem2 == 7);
    
    // Test 3: Divide by 1
    std::array<digit_t, 2> num3 = {123, 456};
    digit_t den3 = 1;
    std::array<digit_t, 2> quo3;
    digit_t rem3;
    mp_fdiv_digit_qr(quo3, rem3, num3, den3);
    TEST(fn_idx, 2, "mp_fdiv_digit_qr: divide by 1", ct_equal(quo3, num3) && rem3 == 0);
    
    // Test 4: Numerator smaller
    std::array<digit_t, 1> num4 = {5};
    digit_t den4 = 10;
    std::array<digit_t, 1> quo4;
    digit_t rem4;
    mp_fdiv_digit_qr(quo4, rem4, num4, den4);
    std::array<digit_t, 1> expected_quo4 = {0};
    TEST(fn_idx, 3, "mp_fdiv_digit_qr: 5 / 10 = 0 r 5", ct_equal(quo4, expected_quo4) && rem4 == 5);
    
    // Test 5: Large values
    std::array<digit_t, 2> num5 = {MAX_DIGIT, MAX_DIGIT};
    digit_t den5 = MAX_DIGIT;
    std::array<digit_t, 2> quo5;
    digit_t rem5;
    mp_fdiv_digit_qr(quo5, rem5, num5, den5);
    // Verify reconstruction
    std::array<digit_t, 3> verify5;
    mp_mul_digit(verify5, quo5, den5);
    verify5[0] += rem5;
    if (verify5[0] < rem5) verify5[1]++;
    std::array<digit_t, 2> result5;
    copy(result5, verify5);
    TEST(fn_idx, 4, "mp_fdiv_digit_qr: large verify", ct_equal(result5, num5) && verify5[2] == 0 && rem5 < den5);
}

// Test lshift: left shift
void test_lshift() {
    const int fn_idx = 19;
    
    // Test 1: Shift by 0
    std::array<digit_t, 2> a1 = {123, 456};
    std::array<digit_t, 2> r1;
    lshift(r1, a1, 0);
    TEST(fn_idx, 0, "lshift: shift by 0", ct_equal(r1, a1));
    
    // Test 2: Shift by 1 bit
    std::array<digit_t, 2> a2 = {5, 10};
    std::array<digit_t, 2> r2;
    std::array<digit_t, 2> expected2 = {10, 20};
    lshift(r2, a2, 1);
    TEST(fn_idx, 1, "lshift: shift by 1 bit", ct_equal(r2, expected2));
    
    // Test 3: Shift by RADIX (one full word)
    std::array<digit_t, 2> a3 = {123, 456};
    std::array<digit_t, 3> r3;
    std::array<digit_t, 3> expected3 = {0, 123, 456};
    lshift(r3, a3, RADIX);
    TEST(fn_idx, 2, "lshift: shift by RADIX", ct_equal(r3, expected3));
    
    // Test 4: Shift with bit overflow
    std::array<digit_t, 1> a4 = {0x8000000000000000ULL};
    std::array<digit_t, 2> r4;
    std::array<digit_t, 2> expected4 = {0, 1};
    lshift(r4, a4, 1);
    TEST(fn_idx, 3, "lshift: bit overflow", ct_equal(r4, expected4));
    
    // Test 5: Shift by RADIX + 1
    std::array<digit_t, 1> a5 = {5};
    std::array<digit_t, 3> r5;
    std::array<digit_t, 3> expected5 = {0, 10, 0};
    lshift(r5, a5, RADIX + 1);
    TEST(fn_idx, 4, "lshift: RADIX + 1", ct_equal(r5, expected5));
    
    // Test 6: Large shift
    std::array<digit_t, 1> a6 = {1};
    std::array<digit_t, 3> r6;
    std::array<digit_t, 3> expected6 = {0, 0, 1};
    lshift(r6, a6, 2 * RADIX);
    TEST(fn_idx, 5, "lshift: 2*RADIX", ct_equal(r6, expected6));
}

// Test rshift: right shift
void test_rshift() {
    const int fn_idx = 20;
    
    // Test 1: Shift by 0
    std::array<digit_t, 2> a1 = {123, 456};
    std::array<digit_t, 2> r1;
    rshift(r1, a1, 0);
    TEST(fn_idx, 0, "rshift: shift by 0", ct_equal(r1, a1));
    
    // Test 2: Shift by 1 bit
    std::array<digit_t, 2> a2 = {10, 20};
    std::array<digit_t, 2> r2;
    std::array<digit_t, 2> expected2 = {5, 10};
    rshift(r2, a2, 1);
    TEST(fn_idx, 1, "rshift: shift by 1 bit", ct_equal(r2, expected2));
    
    // Test 3: Shift by RADIX (one full word)
    std::array<digit_t, 3> a3 = {123, 456, 789};
    std::array<digit_t, 2> r3;
    std::array<digit_t, 2> expected3 = {456, 789};
    rshift(r3, a3, RADIX);
    TEST(fn_idx, 2, "rshift: shift by RADIX", ct_equal(r3, expected3));
    
    // Test 4: Shift with bit underflow
    std::array<digit_t, 2> a4 = {0, 1};
    std::array<digit_t, 1> r4;
    std::array<digit_t, 1> expected4 = {0x8000000000000000ULL};
    rshift(r4, a4, 1);
    TEST(fn_idx, 3, "rshift: bit underflow", ct_equal(r4, expected4));
    
    // Test 5: Shift by RADIX + 1
    std::array<digit_t, 3> a5 = {1, 10, 0};
    std::array<digit_t, 1> r5;
    std::array<digit_t, 1> expected5 = {5};
    rshift(r5, a5, RADIX + 1);
    TEST(fn_idx, 4, "rshift: RADIX + 1", ct_equal(r5, expected5));
    
    // Test 6: Large shift (should result in zero)
    std::array<digit_t, 2> a6 = {123, 456};
    std::array<digit_t, 1> r6;
    std::array<digit_t, 1> expected6 = {0};
    rshift(r6, a6, 3 * RADIX);
    TEST(fn_idx, 5, "rshift: large shift to zero", ct_equal(r6, expected6));
}

// Test mp_compare: comparison
void test_mp_compare() {
    const int fn_idx = 21;
    
    // Test 1: Equal arrays
    std::array<digit_t, 2> a1 = {123, 456};
    std::array<digit_t, 2> b1 = {123, 456};
    TEST(fn_idx, 0, "mp_compare: equal", mp_compare(a1, b1) == 0);
    
    // Test 2: First less than second
    std::array<digit_t, 2> a2 = {100, 200};
    std::array<digit_t, 2> b2 = {100, 300};
    TEST(fn_idx, 1, "mp_compare: a < b", mp_compare(a2, b2) < 0);
    
    // Test 3: First greater than second
    std::array<digit_t, 2> a3 = {100, 300};
    std::array<digit_t, 2> b3 = {100, 200};
    TEST(fn_idx, 2, "mp_compare: a > b", mp_compare(a3, b3) > 0);
    
    // Test 4: High word determines
    std::array<digit_t, 2> a4 = {MAX_DIGIT, 1};
    std::array<digit_t, 2> b4 = {0, 2};
    TEST(fn_idx, 3, "mp_compare: high word matters", mp_compare(a4, b4) < 0);
    
    // Test 5: Zero vs non-zero
    std::array<digit_t, 2> a5 = {0, 0};
    std::array<digit_t, 2> b5 = {1, 0};
    TEST(fn_idx, 4, "mp_compare: zero < one", mp_compare(a5, b5) < 0);
    
    // Test 6: Single word
    std::array<digit_t, 1> a6 = {42};
    std::array<digit_t, 1> b6 = {42};
    TEST(fn_idx, 5, "mp_compare: single word equal", mp_compare(a6, b6) == 0);
    
    // Test 7: MAX values
    std::array<digit_t, 2> a7 = {MAX_DIGIT, MAX_DIGIT};
    std::array<digit_t, 2> b7 = {MAX_DIGIT, MAX_DIGIT - 1};
    TEST(fn_idx, 6, "mp_compare: MAX values", mp_compare(a7, b7) > 0);
    
    // Test 8: Different low, same high
    std::array<digit_t, 2> a8 = {100, 500};
    std::array<digit_t, 2> b8 = {200, 500};
    TEST(fn_idx, 7, "mp_compare: different low", mp_compare(a8, b8) < 0);
}

// Test is_zero: check if array is zero
void test_is_zero() {
    const int fn_idx = 22;
    
    // Test 1: Zero array
    std::array<digit_t, 2> a1 = {0, 0};
    TEST(fn_idx, 0, "is_zero: zero array", is_zero(a1));
    
    // Test 2: Non-zero (low word)
    std::array<digit_t, 2> a2 = {1, 0};
    TEST(fn_idx, 1, "is_zero: non-zero low", !is_zero(a2));
    
    // Test 3: Non-zero (high word)
    std::array<digit_t, 2> a3 = {0, 1};
    TEST(fn_idx, 2, "is_zero: non-zero high", !is_zero(a3));
    
    // Test 4: Large zero array
    std::array<digit_t, 5> a4 = {0, 0, 0, 0, 0};
    TEST(fn_idx, 3, "is_zero: large zero", is_zero(a4));
}

// Test is_one: check if array equals one
void test_is_one() {
    const int fn_idx = 23;
    
    // Test 1: One
    std::array<digit_t, 2> a1 = {1, 0};
    TEST(fn_idx, 0, "is_one: one", is_one(a1));
    
    // Test 2: Zero
    std::array<digit_t, 2> a2 = {0, 0};
    TEST(fn_idx, 1, "is_one: zero", !is_one(a2));
    
    // Test 3: Not one (larger value)
    std::array<digit_t, 2> a3 = {2, 0};
    TEST(fn_idx, 2, "is_one: two", !is_one(a3));
    
    // Test 4: High word non-zero
    std::array<digit_t, 2> a4 = {1, 1};
    TEST(fn_idx, 3, "is_one: high word set", !is_one(a4));
}

// Test equal: array equality
void test_equal() {
    const int fn_idx = 24;
    
    // Test 1: Equal arrays
    std::array<digit_t, 2> a1 = {123, 456};
    std::array<digit_t, 2> b1 = {123, 456};
    TEST(fn_idx, 0, "equal: equal arrays", equal(a1, b1));
    
    // Test 2: Different low word
    std::array<digit_t, 2> a2 = {123, 456};
    std::array<digit_t, 2> b2 = {124, 456};
    TEST(fn_idx, 1, "equal: different low", !equal(a2, b2));
    
    // Test 3: Different high word
    std::array<digit_t, 2> a3 = {123, 456};
    std::array<digit_t, 2> b3 = {123, 457};
    TEST(fn_idx, 2, "equal: different high", !equal(a3, b3));
    
    // Test 4: Both zero
    std::array<digit_t, 3> a4 = {0, 0, 0};
    std::array<digit_t, 3> b4 = {0, 0, 0};
    TEST(fn_idx, 3, "equal: both zero", equal(a4, b4));
}

// Test copy: array copy
void test_copy() {
    const int fn_idx = 25;
    
    // Test 1: Same size copy
    std::array<digit_t, 3> a1 = {1, 2, 3};
    std::array<digit_t, 3> r1;
    copy(r1, a1);
    TEST(fn_idx, 0, "copy: same size", ct_equal(r1, a1));
    
    // Test 2: Smaller to larger (zero-pad)
    std::array<digit_t, 2> a2 = {100, 200};
    std::array<digit_t, 4> r2;
    std::array<digit_t, 4> expected2 = {100, 200, 0, 0};
    copy(r2, a2);
    TEST(fn_idx, 1, "copy: zero-pad", ct_equal(r2, expected2));
    
    // Test 3: Larger to smaller (truncate)
    std::array<digit_t, 4> a3 = {1, 2, 3, 4};
    std::array<digit_t, 2> r3;
    std::array<digit_t, 2> expected3 = {1, 2};
    copy(r3, a3);
    TEST(fn_idx, 2, "copy: truncate", ct_equal(r3, expected3));
    
    // Test 4: Copy zeros
    std::array<digit_t, 3> a4 = {0, 0, 0};
    std::array<digit_t, 3> r4;
    copy(r4, a4);
    TEST(fn_idx, 3, "copy: zeros", ct_equal(r4, a4));
}

// Test zero: array zeroing
void test_zero() {
    const int fn_idx = 26;
    
    // Test 1: Zero small array
    std::array<digit_t, 2> r1 = {123, 456};
    std::array<digit_t, 2> expected1 = {0, 0};
    zero(r1);
    TEST(fn_idx, 0, "zero: small array", ct_equal(r1, expected1));
    
    // Test 2: Zero large array
    std::array<digit_t, 5> r2 = {1, 2, 3, 4, 5};
    std::array<digit_t, 5> expected2 = {0, 0, 0, 0, 0};
    zero(r2);
    TEST(fn_idx, 1, "zero: large array", ct_equal(r2, expected2));
    
    // Test 3: Zero single word
    std::array<digit_t, 1> r3 = {MAX_DIGIT};
    std::array<digit_t, 1> expected3 = {0};
    zero(r3);
    TEST(fn_idx, 2, "zero: single word", ct_equal(r3, expected3));
}

// Test ct_bitsize: constant-time bit size calculation
void test_ct_bitsize() {
    const int fn_idx = 27;
    
    // Test 1: Zero has size 0
    digit_t d1 = 0;
    TEST(fn_idx, 0, "ct_bitsize: zero digit", ct_bitsize(d1) == 0);
    
    // Test 2: One has size 1
    digit_t d2 = 1;
    TEST(fn_idx, 1, "ct_bitsize: one", ct_bitsize(d2) == 1);
    
    // Test 3: Power of 2
    digit_t d3 = (1ULL << (RADIX - 1));
    TEST(fn_idx, 2, "ct_bitsize: 2^63", ct_bitsize(d3) == (RADIX));
    
    // Test 4: MAX_DIGIT
    digit_t d4 = MAX_DIGIT;
    TEST(fn_idx, 3, "ct_bitsize: MAX_DIGIT", ct_bitsize(d4) == RADIX);
    
    // Test 5: Array - zero
    std::array<digit_t, 2> a5 = {0, 0};
    TEST(fn_idx, 4, "ct_bitsize: zero array", ct_bitsize(a5) == 0);
    
    // Test 6: Array - low word only
    std::array<digit_t, 2> a6 = {255, 0};
    TEST(fn_idx, 5, "ct_bitsize: low word", ct_bitsize(a6) == 8);
    
    // Test 7: Array - high word
    std::array<digit_t, 2> a7 = {0, 1};
    TEST(fn_idx, 6, "ct_bitsize: high word", ct_bitsize(a7) == 65);
    
    // Test 8: Array - full
    std::array<digit_t, 2> a8 = {MAX_DIGIT, MAX_DIGIT};
    TEST(fn_idx, 7, "ct_bitsize: all bits", ct_bitsize(a8) == 128);
}

// Main test runner
int run_tests() {
    const char* function_names[NUM_TESTS] = {
        "mp_add          ", "mp_addc         ", "mp_addc_digit   ", "mp_add_div      ", "mp_addc_div     ",
        "mp_sub          ", "mp_subc         ", "mp_sub_cond     ", "mp_subc_cond    ",
        "mp_mul          ", "mp_mul_digit    ", "mp_mul_high     ",
        "mp_div_qr       ", "mp_div_q        ", "mp_div_r        ",
        "mp_div_digit    ", "mp_div_digit_q  ", "mp_div_digit_r  ", "mp_fdiv_digit   ",
        "lshift          ", "rshift          ",
        "mp_compare      ", "is_zero         ", "is_one          ", "equal           ",
        "copy            ", "zero            ", "ct_bitsize      "
    };
    
    // Run all test functions
    test_mp_add();
    test_mp_addc();
    test_mp_addc_digit();
    test_mp_add_and_divide();
    test_mp_addc_and_divide();
    test_mp_sub();
    test_mp_subc();
    test_mp_sub_conditional();
    test_mp_mul();
    test_mp_mul_digit();
    test_mp_mul_high();
    test_mp_div_qr();
    test_mp_div_q();
    test_mp_div_r();
    test_mp_div_digit_qr();
    test_mp_div_digit_q();
    test_mp_div_digit_r();
    test_mp_fdiv_digit_qr();
    test_lshift();
    test_rshift();
    test_mp_compare();
    test_is_zero();
    test_is_one();
    test_equal();
    test_copy();
    test_zero();
    test_ct_bitsize();
    
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
    int failed_count = 0;
    int skipped_fns = 0;
    
    for(int i = 0; i < NUM_TESTS; i++) {
        if (function_skipped[i]) {
            skipped_fns++;
            continue;
        }
        for(int j = 0; j < TESTS_LEN(i); j++) {
            total_tests++;
            if (tests[i][j]) failed_count++;
        }
    }
    
    if(failed_count == 0 && skipped_fns == 0) {
        std::cout << "✔ All " << total_tests << " tests passed!\n\n";
        return 0;
    } else {
        if (failed_count > 0) {
            std::cout << "✗ " << failed_count << " of " << total_tests << " tests failed\n";
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
    std::cout << "Testing MP Library\n";
    std::cout << "RADIX = " << RADIX << " bits\n";
    std::cout << "=====================\n\n";
    
    int result = run_tests();
    
    return result;
}
