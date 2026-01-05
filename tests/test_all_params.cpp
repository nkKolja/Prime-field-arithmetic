#include "params.hpp"
#include "primes/p64_0.hpp"
#include "primes/p64_1.hpp"
#include "primes/p128_0.hpp"
#include "primes/p128_1.hpp"
#include "primes/p192_0.hpp"
#include "primes/p192_1.hpp"
#include "primes/p256_0.hpp"
#include "primes/p256_1.hpp"
#include "primes/p512_0.hpp"
#include "primes/p512_1.hpp"
#include <iostream>
#include <iomanip>

// Define all 10 primes using the new interface
DEFINE_PRIME_FROM_ARRAY(P64_0_Computed, 0x1FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P64_1_Computed, 0xFFFFFFFFFFFFFFC5);
DEFINE_PRIME_FROM_ARRAY(P128_0_Computed, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P128_1_Computed, 0xFFFFFFFFFFFFFF53, 0xFFFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P192_0_Computed, 0xFFFFFFFFFFFFFF13, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P192_1_Computed, 0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P256_0_Computed, 0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P256_1_Computed, 0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFF, 0x0000000000000000, 0xFFFFFFFF00000001);
DEFINE_PRIME_FROM_ARRAY(P512_0_Computed, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P512_1_Computed, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);

template<typename Computed, typename Expected>
bool check_array(const char* name, const char* param_name, 
                 const std::array<prime_field::digit_t, Computed::NWORDS>& computed,
                 const std::array<prime_field::digit_t, Expected::NWORDS>& expected) {
    bool match = true;
    for (size_t i = 0; i < Computed::NWORDS; i++) {
        if (computed[i] != expected[i]) {
            match = false;
            break;
        }
    }
    
    if (!match) {
        std::cout << "✗ " << name << " " << param_name << " MISMATCH:\n";
        std::cout << "  Computed: ";
        for (int i = Computed::NWORDS - 1; i >= 0; i--) {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(16) << computed[i];
            if (i > 0) std::cout << " ";
        }
        std::cout << "\n  Expected: ";
        for (int i = Expected::NWORDS - 1; i >= 0; i--) {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(16) << expected[i];
            if (i > 0) std::cout << " ";
        }
        std::cout << "\n";
    }
    
    return match;
}

template<typename Computed, typename Expected>
bool check_prime(const char* name) {
    bool all_ok = true;
    
    all_ok &= check_array<Computed, Expected>(name, "p", Computed::p, Expected::p);
    all_ok &= check_array<Computed, Expected>(name, "Mont_one", Computed::Mont_one, Expected::Mont_one);
    all_ok &= check_array<Computed, Expected>(name, "R2", Computed::R2, Expected::R2);
    all_ok &= check_array<Computed, Expected>(name, "pp", Computed::pp, Expected::pp);
    all_ok &= check_array<Computed, Expected>(name, "ip", Computed::ip, Expected::ip);
    all_ok &= check_array<Computed, Expected>(name, "iR", Computed::iR, Expected::iR);
    all_ok &= check_array<Computed, Expected>(name, "TWO_POW_NBITS", Computed::TWO_POW_NBITS, Expected::TWO_POW_NBITS);
    
    if (all_ok) {
        std::cout << "✓ " << name << " - ALL PARAMETERS MATCH\n";
    } else {
        std::cout << "✗ " << name << " - ERRORS FOUND\n";
    }
    
    return all_ok;
}

int main() {
    using namespace prime_field;
    
    std::cout << "Testing ALL Montgomery parameters for ALL primes:\n";
    std::cout << "==================================================\n\n";
    
    if (!check_prime<P64_0_Computed, P64_0>("P64_0")) return 1;
    if (!check_prime<P64_1_Computed, P64_1>("P64_1")) return 1;
    if (!check_prime<P128_0_Computed, P128_0>("P128_0")) return 1;
    if (!check_prime<P128_1_Computed, P128_1>("P128_1")) return 1;
    if (!check_prime<P192_0_Computed, P192_0>("P192_0")) return 1;
    if (!check_prime<P192_1_Computed, P192_1>("P192_1")) return 1;
    if (!check_prime<P256_0_Computed, P256_0>("P256_0")) return 1;
    if (!check_prime<P256_1_Computed, P256_1>("P256_1")) return 1;
    if (!check_prime<P512_0_Computed, P512_0>("P512_0")) return 1;
    if (!check_prime<P512_1_Computed, P512_1>("P512_1")) return 1;
    
    std::cout << "\n==================================================\n";
    std::cout << "ALL PRIMES PASSED!\n";
    
    return 0;
}
