#include <random>
#include <iostream>
#include "../include/mp/mp.hpp"
#include "../include/prime_field/montgomery.hpp"
#include "../include/prime_field/params.hpp"
#include "../include/prime_field/field_element.hpp"
#include "../include/prime_field/primes/p32_0.hpp"
#include "../include/prime_field/primes/p64_0.hpp"
#include "../include/prime_field/primes/p128_0.hpp"
#include "../include/prime_field/primes/p192_0.hpp"
#include "../include/prime_field/primes/p256_0.hpp"
#include "../include/prime_field/primes/p512_0.hpp"

using namespace common;
using namespace mp;
using namespace prime_field;

// Random number generator
std::random_device rd;
std::mt19937_64 rng(rd());

// Helper to generate random digit
digit_t random_digit() {
    return rng();
}

// Template helper to test Montgomery reduction for a given prime
template<typename Prime>
bool test_montgomery_for_prime() {
    constexpr size_t N = Prime::NWORDS;
    constexpr auto& p = Prime::p;
    constexpr auto& iR = Prime::iR;

    // Test 1: Zero value
    {
        std::array<digit_t, 2*N> c = {};
        std::array<digit_t, N> out;
        montgomery_reduce<Prime>(out, c);

        std::array<digit_t, 2*N + N> check;
        mp_mul(check, c, iR);
        std::array<digit_t, N> expected = {};
        mp_div_r<N, 2*N + N>(expected, check, p);
        
        if (!ct_equal(out, expected)) {
            std::cout << "  FAILED at test 0 (zero value)" << std::endl;
            std::cout << "  Input c: " << c << std::endl;
            std::cout << "  Input p: " << p << std::endl;
            std::cout << "  Input iR: " << iR << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Got: " << out << std::endl;
            return false;
        }
    }

    // Test 2-51: Random values
    for (int i = 0; i < 50; i++) {
        std::array<digit_t, 2*N> c;
        FieldElement<Prime> temp = random<Prime>();
        
        for (size_t j = 0; j < N; j++) {
            c[j] = random_digit();
        }
        for (size_t j = 0; j < N; j++) {
            c[j + N] = temp.data[j];
        }

        std::array<digit_t, N> out;
        montgomery_reduce<Prime>(out, c);
        

        std::array<digit_t, 2*N + N> check;
        mp_mul(check, c, iR);
        std::array<digit_t, N> expected;
        mp_div_r<N, 2*N + N>(expected, check, p);

        if (!ct_equal(out, expected)) {
            std::cout << "  FAILED at iteration " << i << std::endl;
            std::cout << "  Input c: " << c << std::endl;
            std::cout << "  Input p: " << p << std::endl;
            std::cout << "  Input iR: " << iR << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Got: " << out << std::endl;
            return false;
        }
    }

    return true;
}

// Template helper to test Barrett reduction for a given prime and A value
template<typename Prime, size_t A>
bool test_barrett_for_prime_and_A() {
    constexpr size_t N = Prime::NWORDS;
    constexpr auto& p = Prime::p;
    
    // Test 1: Zero value
    {
        std::array<digit_t, N + A> c = {};
        std::array<digit_t, N> out;
        barrett_reduce<Prime, A>(out, c);
        
        std::array<digit_t, N> expected;
        mp_div_r<N, N + A>(expected, c, p);
        
        if (!ct_equal(out, expected)) {
            std::cout << "  FAILED at test 0 (zero value)" << std::endl;
            std::cout << "  Input c: " << c << std::endl;
            std::cout << "  Input p: " << p << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Got: " << out << std::endl;
            return false;
        }
    }
    
    // Test 2-51: Random values
    for (int i = 0; i < 50; i++) {
        std::array<digit_t, N + A> c;
        for (size_t j = 0; j < N + A; j++) {
            c[j] = random_digit();
        }
        
        std::array<digit_t, N> out;
        barrett_reduce<Prime, A>(out, c);
        
        std::array<digit_t, N> expected;
        mp_div_r<N, N + A>(expected, c, p);
        
        if (!ct_equal(out, expected)) {
            std::cout << "  FAILED at iteration " << i << std::endl;
            std::cout << "  Input c: " << c << std::endl;
            std::cout << "  Input p: " << p << std::endl;
            std::cout << "  Input p2: " << Prime::px2_overflow << std::endl;
            std::cout << "  Input p3: " << Prime::px3_overflow << std::endl;
            std::cout << "  Barrett factor: " << prime_field::montgomery::compute_barrett_appx_factor<N, A>(p) << std::endl;
            std::cout << "  Expected: " << expected << std::endl;
            std::cout << "  Got: " << out << std::endl;
            return false;
        }
    }
    
    return true;
}

// Test Montgomery reduction
void test_montgomery_reduce() {
    if (!test_montgomery_for_prime<P32_0>()) exit(1);
    if (!test_montgomery_for_prime<P64_0>()) exit(1);
    if (!test_montgomery_for_prime<P128_0>()) exit(1);
    if (!test_montgomery_for_prime<P192_0>()) exit(1);
    if (!test_montgomery_for_prime<P256_0>()) exit(1);
    if (!test_montgomery_for_prime<P512_0>()) exit(1);
}

// Helper to test all A values from 1 to min(8, 3*N) for a given prime
template<typename Prime, size_t A = 1>
bool test_barrett_all_A() {
    constexpr size_t N = Prime::NWORDS;
    constexpr size_t MAX_A = (8 < 3 * N) ? 8 : 3 * N;
    
    if (!test_barrett_for_prime_and_A<Prime, A>()) {
        return false;
    }
    if constexpr (A < MAX_A) {
        return test_barrett_all_A<Prime, A + 1>();
    }
    return true;
}

// Test Barrett reduction
void test_barrett_reduce() {
    if (!test_barrett_all_A<P32_0>()) exit(1);
    if (!test_barrett_all_A<P64_0>()) exit(1);
    if (!test_barrett_all_A<P128_0>()) exit(1);
    if (!test_barrett_all_A<P192_0>()) exit(1);
    if (!test_barrett_all_A<P256_0>()) exit(1);
    if (!test_barrett_all_A<P512_0>()) exit(1);
}

int main() {
    // Run tests
    test_montgomery_reduce();
    test_barrett_reduce();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
