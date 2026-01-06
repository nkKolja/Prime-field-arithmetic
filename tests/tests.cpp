#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include "prime_field/field_element.hpp"

// Include all prime definitions
#include "prime_field/primes/p64_0.hpp"
#include "prime_field/primes/p64_1.hpp"
#include "prime_field/primes/p128_0.hpp"
#include "prime_field/primes/p128_1.hpp"
#include "prime_field/primes/p192_0.hpp"
#include "prime_field/primes/p192_1.hpp"
#include "prime_field/primes/p256_0.hpp"
#include "prime_field/primes/p256_1.hpp"
#include "prime_field/primes/p512_0.hpp"
#include "prime_field/primes/p512_1.hpp"

using namespace prime_field;

#define TEST_LOOPS 256

#define RED_TESTS   3
#define ADD_TESTS   3
#define NEG_TESTS   3
#define SUB_TESTS   5
#define MUL_TESTS   5
#define LEG_TESTS   3
#define INV_TESTS   2
#define SQRT_TESTS  1
#define NUM_TESTS   8

#define TESTS_PAD   5
#define TESTS_LEN(x) (  (x) == 0 ? RED_TESTS  : \
                        (x) == 1 ? ADD_TESTS  : \
                        (x) == 2 ? NEG_TESTS  : \
                        (x) == 3 ? SUB_TESTS  : \
                        (x) == 4 ? MUL_TESTS  : \
                        (x) == 5 ? LEG_TESTS  : \
                        (x) == 6 ? INV_TESTS  : \
                        (x) == 7 ? SQRT_TESTS : -1)

#define PASS(x) ((x) ? "\033[31m✗\033[0m" : "\033[0;32m✔\033[0m")

const char* pass_check(bool arr[], int n) {
    static char buffer[256] = "";
    buffer[0] = '\0';
    for(int i = 0; i < TESTS_PAD - n; i++)
        strcat(buffer, " ");
    for(int i = 0; i < n; i++)
        strcat(buffer, PASS(arr[i]));
    return buffer;
}

template<typename Prime>
int run_tests() {
    using F = FieldElement<Prime>;
    
    const char *function_names[] = {"f_red", "f_add", "f_neg", "f_sub", "f_mul", "f_leg", "f_inv", "f_sqr"};
    bool tests[NUM_TESTS][256] = {{0}};
    int result = 0, first_leg = 0;

    // Allocate test vectors
    std::vector<F> t0(TEST_LOOPS);
    std::vector<F> t1(TEST_LOOPS);
    std::vector<F> t2(TEST_LOOPS);

    // Generate random values
    for(int i = 0; i < TEST_LOOPS; i++) {
        t0[i] = random<Prime>();
        t1[i] = random<Prime>();
        t2[i] = random<Prime>();
    }

    // Special values
    F max_val;
    for(size_t i = 0; i < Prime::NWORDS; i++)
        max_val.data[i] = 0xFFFFFFFFFFFFFFFF;
    
    F Rm1;
    Rm1.data = Prime::Mont_one;
    Rm1.data[0] -= 1;
    
    F zero_elem;
    for(size_t i = 0; i < Prime::NWORDS; i++)
        zero_elem.data[i] = 0;
    
    F one_mont;
    one_mont.data = Prime::Mont_one;
    
    F p_elem;
    p_elem.data = Prime::p;

    std::cout << "\n";

    for(int i = 0; i < TEST_LOOPS; i++) {
        F s0, s1, s2;
        unsigned char s[3];
        int l[3];

        // Reduction
        // t0 reduced => t0 % p = t0
        s0 = t0[i]; reduce(s0);
        tests[0][0] |= (s0 != t0[i]);

        // p % p = 0
        s0 = p_elem; reduce(s0);
        tests[0][1] |= (s0 != zero_elem);

        // (R - 1) % p = Rm1
        s0 = max_val; reduce_full(s0);
        tests[0][2] |= (s0 != Rm1);

        // Addition
        // t0 + 0 = t0
        add(s0, t0[i], zero_elem);
        tests[1][0] |= (s0 != t0[i]);

        // (t0 + t1) + t2 = t0 + (t1 + t2)
        add(s0, t0[i], t1[i]); add(s0, s0, t2[i]);
        add(s1, t1[i], t2[i]); add(s1, t0[i], s1);
        tests[1][1] |= (s0 != s1);

        // t0 + t1 = t1 + t0
        add(s0, t0[i], t1[i]);
        add(s1, t1[i], t0[i]);
        tests[1][2] |= (s0 != s1);


        // Negation
        // -0 = 0
        neg(s1, zero_elem);
        tests[2][0] |= (s1 != zero_elem);

        // (-t0) + t0 = 0
        neg(s0, t0[i]); add(s1, s0, t0[i]);
        tests[2][1] |= (s1 != zero_elem);

        // t0 + (-t0) = 0
        neg(s0, t0[i]); add(s1, t0[i], s0);
        tests[2][2] |= (s1 != zero_elem);


        // Subtraction
        // t0 - 0 = t0
        sub(s0, t0[i], zero_elem);
        tests[3][0] |= (s0 != t0[i]);

        // 0 - t0 = -t0
        sub(s0, zero_elem, t0[i]);
        neg(s1, t0[i]);
        tests[3][1] |= (s0 != s1);

        // (t0 - t1) - t2 = (t0 - t2) - t1 = t0 - (t1 + t2)
        sub(s0, t0[i], t1[i]); sub(s0, s0, t2[i]);
        sub(s1, t0[i], t2[i]); sub(s1, s1, t1[i]);
        add(s2, t1[i], t2[i]); sub(s2, t0[i], s2);
        tests[3][2] |= (s0 != s1);
        tests[3][3] |= (s0 != s2);

        // t0 - t1 = -(t1 - t0)
        sub(s0, t0[i], t1[i]);
        sub(s1, t1[i], t0[i]); neg(s1, s1);
        tests[3][4] |= (s0 != s1);

        // Multiplication
        // t0 * 0 = 0
        mul(s0, t0[i], zero_elem);
        tests[4][0] |= (s0 != zero_elem);

        // t0 * t1 = t0
        mul(s0, t0[i], one_mont);
        tests[4][1] |= (s0 != t0[i]);

        // ( t0 + t1 ) * t2 = t0 * t2 + t1 * t2
        add(s0, t0[i], t1[i]); mul(s0, s0, t2[i]);
        mul(s1, t0[i], t2[i]); mul(s2, t1[i], t2[i]); add(s1, s1, s2);
        tests[4][2] |= (s0 != s1);

        // (t0 * t1) * t2 = t0 * (t1 * t2)
        mul(s0, t0[i], t1[i]); mul(s0, s0, t2[i]);
        mul(s1, t1[i], t2[i]); mul(s1, t0[i], s1);
        tests[4][3] |= (s0 != s1);

        // t0 * t1 = t1 * t0
        mul(s0, t0[i], t1[i]);
        mul(s1, t1[i], t0[i]);
        tests[4][4] |= (s0 != s1);


        // Legendre symbol
        // legendre(t0 * t0) = 1
        mul(s0, t0[i], t0[i]); l[0] = legendre(s0);
        if(t0[i] != zero_elem)
            tests[5][0] |= (l[0] != 1);

        // legendre(t0 * t1) = legendre(t0) * legendre(t1)
        l[0] = legendre(t0[i]);
        l[1] = legendre(t1[i]);
        mul(s0, t0[i], t1[i]); l[2] = legendre(s0);
        if(t0[i] != zero_elem && t1[i] != zero_elem)
            tests[5][1] |= ((l[0] * l[1] * l[2]) != 1);

        if(i == 0) first_leg = l[0];
        tests[5][2] |= (first_leg != l[0]) | (first_leg != l[1]) | (first_leg != l[2]);
        if(i == TEST_LOOPS - 1) tests[5][2] = !tests[5][2];


        // Inverse
        // inv(t0) * t0 = 1
        inv(s0, t0[i]);
        mul(s0, s0, t0[i]);
        if(t0[i] != zero_elem)
            tests[6][0] |= (s0 != one_mont);

        // inv(inv(t0)) = t0
        inv(s0, t0[i]);
        inv(s1, s0);
        tests[6][1] |= (s1 != t0[i]);


        // Square root
        // sqrt(t0)^2 = t0 for quadratic residues
        sqrt(s0, t0[i]);
        mul(s0, s0, s0);
        s[0] = legendre(t0[i]);
        if(s[0] == 1)
            tests[7][0] |= (t0[i] != s0);
    }

    // Print results
    for(int j = 0; j < NUM_TESTS; j++) {
        std::cout << function_names[j] << "   " << pass_check(tests[j], TESTS_LEN(j)) << "\n";
    }
    std::cout << "\n";

    for(int i = 0; i < NUM_TESTS; i++)
        for(int j = 0; j < TESTS_LEN(i); j++)
            result |= tests[i][j];

    if(result == 0)
        std::cout << "✔ All tests passed!\n\n";
    else
        std::cout << "✗ Some tests failed!\n\n";
    
    return result;
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

int main() {
#ifdef PRIME_TYPE
    std::cout << "\n";
    std::cout << "===================\n";
    std::cout << "Testing " << TOSTRING(PRIME_TYPE) << "\n";
    
    int result = run_tests<PRIME_TYPE>();
    std::cout << "\n";
    return result;
#else
    #error "PRIME_TYPE must be defined (e.g., -DPRIME_TYPE=P64_0)"
#endif
}
