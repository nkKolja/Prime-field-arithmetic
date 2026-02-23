#pragma once

#include <cstdint>
#include <array>

// Prime identifiers
#define P32_0_ID   0
#define P64_0_ID   1
#define P64_1_ID   2
#define P128_0_ID  3
#define P128_1_ID  4
#define P192_0_ID  5
#define P192_1_ID  6
#define P256_0_ID  7
#define P256_1_ID  8
#define P512_0_ID  9
#define P512_1_ID  10

// PRIME_ID is defined at compile time via -DPRIME_ID=<value>
// Default for IDE IntelliSense (will be overridden at build time)
#ifndef PRIME_ID
    #define PRIME_ID P256_0_ID
#endif

// Include the appropriate prime based on PRIME_ID
#if PRIME_ID == P32_0_ID
    #include "prime_field/primes/p32_0.hpp"
    namespace prime_field { using Prime = P32_0; }
#elif PRIME_ID == P64_0_ID
    #include "prime_field/primes/p64_0.hpp"
    namespace prime_field { using Prime = P64_0; }
#elif PRIME_ID == P64_1_ID
    #include "prime_field/primes/p64_1.hpp"
    namespace prime_field { using Prime = P64_1; }
#elif PRIME_ID == P128_0_ID
    #include "prime_field/primes/p128_0.hpp"
    namespace prime_field { using Prime = P128_0; }
#elif PRIME_ID == P128_1_ID
    #include "prime_field/primes/p128_1.hpp"
    namespace prime_field { using Prime = P128_1; }
#elif PRIME_ID == P192_0_ID
    #include "prime_field/primes/p192_0.hpp"
    namespace prime_field { using Prime = P192_0; }
#elif PRIME_ID == P192_1_ID
    #include "prime_field/primes/p192_1.hpp"
    namespace prime_field { using Prime = P192_1; }
#elif PRIME_ID == P256_0_ID
    #include "prime_field/primes/p256_0.hpp"
    namespace prime_field { using Prime = P256_0; }
#elif PRIME_ID == P256_1_ID
    #include "prime_field/primes/p256_1.hpp"
    namespace prime_field { using Prime = P256_1; }
#elif PRIME_ID == P512_0_ID
    #include "prime_field/primes/p512_0.hpp"
    namespace prime_field { using Prime = P512_0; }
#elif PRIME_ID == P512_1_ID
    #include "prime_field/primes/p512_1.hpp"
    namespace prime_field { using Prime = P512_1; }
#else
    #error "Invalid PRIME_ID"
#endif


