#include "params.hpp"
#include <iostream>
#include <iomanip>

// Define all 10 primes using the new interface
DEFINE_PRIME_FROM_ARRAY(P64_0, 0x1FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P64_1, 0xFFFFFFFFFFFFFFC5);
DEFINE_PRIME_FROM_ARRAY(P128_0, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P128_1, 0xFFFFFFFFFFFFFF53, 0xFFFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P192_0, 0xFFFFFFFFFFFFFF13, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P192_1, 0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P256_0, 0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P256_1, 0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFF, 0x0000000000000000, 0xFFFFFFFF00000001);
DEFINE_PRIME_FROM_ARRAY(P512_0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
DEFINE_PRIME_FROM_ARRAY(P512_1, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF);

// Expected values from hardcoded files
struct Expected {
    size_t nwords, nbits;
    std::string name;
};

constexpr Expected expected[] = {
    {1, 61,  "P64_0"},
    {1, 64,  "P64_1"},
    {2, 127, "P128_0"},
    {2, 128, "P128_1"},
    {3, 192, "P192_0"},
    {3, 191, "P192_1"},
    {4, 255, "P256_0"},
    {4, 256, "P256_1"},
    {8, 512, "P512_0"},
    {8, 511, "P512_1"}
};

template<typename Prime, size_t Idx>
void check_prime() {
    bool ok = true;
    
    if (Prime::NWORDS != expected[Idx].nwords) {
        std::cout << "FAIL: " << expected[Idx].name << " NWORDS = " << Prime::NWORDS 
                  << " (expected " << expected[Idx].nwords << ")\n";
        ok = false;
    }
    
    if (Prime::NBITS != expected[Idx].nbits) {
        std::cout << "FAIL: " << expected[Idx].name << " NBITS = " << Prime::NBITS 
                  << " (expected " << expected[Idx].nbits << ")\n";
        ok = false;
    }
    
    if (ok) {
        std::cout << "✓ " << expected[Idx].name 
                  << " (NWORDS=" << Prime::NWORDS 
                  << ", NBITS=" << Prime::NBITS << ")\n";
    }
}

template<typename Prime>
void print_params(const char* name) {
    std::cout << "\n" << name << ":\n";
    std::cout << "  NWORDS = " << Prime::NWORDS << ", NBITS = " << Prime::NBITS << "\n";
    std::cout << "  p         = ";
    for (int i = Prime::NWORDS - 1; i >= 0; i--) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(16) << Prime::p[i];
        if (i > 0) std::cout << " ";
    }
    std::cout << "\n  Mont_one  = ";
    for (int i = Prime::NWORDS - 1; i >= 0; i--) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(16) << Prime::Mont_one[i];
        if (i > 0) std::cout << " ";
    }
    std::cout << "\n  R2        = ";
    for (int i = Prime::NWORDS - 1; i >= 0; i--) {
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(16) << Prime::R2[i];
        if (i > 0) std::cout << " ";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "Testing NWORDS and NBITS computation:\n";
    std::cout << "=====================================\n";
    
    check_prime<P64_0, 0>();
    check_prime<P64_1, 1>();
    check_prime<P128_0, 2>();
    check_prime<P128_1, 3>();
    check_prime<P192_0, 4>();
    check_prime<P192_1, 5>();
    check_prime<P256_0, 6>();
    check_prime<P256_1, 7>();
    check_prime<P512_0, 8>();
    check_prime<P512_1, 9>();
    
    std::cout << "\n\nSample computed Montgomery parameters:";
    print_params<P64_0>("P64_0 (2^61 - 1)");
    print_params<P64_1>("P64_1 (2^64 - 59)");
    print_params<P128_0>("P128_0 (2^127 - 1)");
    print_params<P256_1>("P256_1 (P-256)");
    
    return 0;
}
