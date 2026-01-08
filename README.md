# Prime Field Arithmetic

A C++ template library for constant-time finite field arithmetic using Montgomery representation.

## Overview

A hybrid header-only/pre-compiled library for prime field arithmetic. This is a reimagining of [Finite-field-arithmetic](https://github.com/nkKolja/Finite-field-arithmetic) (C implementation) using modern C++ features.

**Features:**
- **Template metaprogramming** - define your own prime and all Montgomery parameters are computed at compile time
- **Constant-time operations** - no data-dependent branches or operations
- **Constant memory access** - no data-dependent memory address accesses
- **Hybrid distribution** - pre-compiled static libraries for fast compilation, or header-only for custom primes
- **Pre-configured primes** - Optimised primes from 64 to 512 bits (10 total)
- **Field operations** - add, sub, neg, mul, inv, div, pow, legendre, sqrt

## Library Structure

```
Prime-field-arithmetic/
├── include/          # Header files (public API)
│   └── prime_field/  # Namespace-organized headers
├── lib/              # Pre-compiled static libraries
│   ├── libprime_field_p64_0.a
│   ├── libprime_field_p256_0.a
│   └── ... (10 total)
├── src/              # Template instantiations
│   └── primes/       # One .cpp per prime
├── tests/            # Test suite
└── benchmarks/       # Performance benchmarks
```

## Work in progress

- **ARM optimizations not yet implemented** - currently uses generic implementations only
- **Prime-specific optimizations not yet added** - no specialized routines for special-form primes
- The `random()` method is not seedable but uses system cryptographic randomness. Statistical distance from uniform worst case ≤ 2^(-63), but in general much larger.
- The `sqrt()` function is **NOT constant-time** and Cipolla's algorithm is used as a temporary replacement.

## Comparison with C Implementation

| Aspect | C Version | This C++ Version |
|--------|-----------|------------------|
| Montgomery params | Python codegen | `constexpr` functions |
| Prime selection | Compile-time macro | Template parameter |
| Type safety | None | Compile-time enforced |
| Syntax | `f_add(a, b, c)` | `c = a + b` |
| ARM assembly | ✓ Available | ✗ Not yet implemented |
| Prime-specific opts | ✓ For special primes | ✗ Not yet implemented |
| Custom primes | Rebuild required | Define struct + done |

## Pre-configured Prime Fields

The library includes 10 pre-configured prime fields across 5 bit sizes:

### 64-bit Primes

| Config | Prime | Hex Value |
|--------|-------|-----------|
| **P64_0** | 2^61 - 1 | `0x1FFFFFFFFFFFFFFF` |
| **P64_1** | 2^64 - 59 | `0xFFFFFFFFFFFFFFC5` |

### 128-bit Primes

| Config | Prime | Hex Value |
|--------|-------|-----------|
| **P128_0** | 2^127 - 1 | `0x7FFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF` |
| **P128_1** | 2^128 - 173 | `0xFFFFFFFFFFFFFFFF FFFFFFFFFFFFFF53` |

### 192-bit Primes

| Config | Prime | Hex Value |
|--------|-------|-----------|
| **P192_0** | 2^192 - 237 | `0xFFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFF13` |
| **P192_1** | 2^191 - 19 | `0x7FFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFED` |

### 256-bit Primes

| Config | Prime | Hex Value |
|--------|-------|-----------|
| **P256_0** | 2^255 - 19 (Curve25519) | `0x7FFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFED` |
| **P256_1** | 2^256 - 2^224 + 2^192 + 2^96 - 1 (P-256/secp256r1) | `0xFFFFFFFF00000001 0000000000000000 00000000FFFFFFFF FFFFFFFFFFFFFFFF` |

### 512-bit Primes

| Config | Prime | Hex Value |
|--------|-------|-----------|
| **P512_0** | 2^512 - 2^256 + 2^192 - 2^128 - 1 | `0xFFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF 0000000000000000 FFFFFFFFFFFFFFFE FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF` |
| **P512_1** | 2^511 - 2^320 + 1 | `0x7FFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF FFFFFFFFFFFFFFFF 0000000000000000 0000000000000000 0000000000000000 0000000000000000 0000000000000001` |

## Usage

### Option 1: Link Against Pre-compiled Libraries (Fast Compilation)

```cpp
#include "prime_field.hpp"

using namespace prime_field;
using namespace prime_field::montgomery;

// Use a pre-configured prime
using Field = P256_0;  // Curve25519 prime

FieldElement<Field> a(5);
FieldElement<Field> b(7);
FieldElement<Field> c = a * b + a;

```

**Compile and link:**
```bash
g++ -std=c++17 -O3 -I./include your_code.cpp -L./lib -lprime_field_p256_0
```

### Defining Custom Primes

```cpp
#include "prime_field.hpp"

using namespace prime_field::montgomery;

// Define a custom prime (e.g., 2^64 - 97)
using MyPrime = PrimeParameters<0xFFFFFFFFFFFFFF9F>;

// All Montgomery parameters computed at compile time
FieldElement<MyPrime> x(42);
FieldElement<MyPrime> y = x * x;
```

**Compile:**
```bash
g++ -std=c++17 -O3 -I./include your_code.cpp
```

### Available Operations

```cpp
FieldElement<Field> a, b, result;

// Arithmetic
add(result, a, b);      // result = a + b
sub(result, a, b);      // result = a - b  
mul(result, a, b);      // result = a * b
inv(result, a);         // result = a^(-1)
pow(result, a, exp);    // result = a^exp (constant-time)

// Or use operators
result = a + b;
result = a - b;
result = a * b;
result = -a;

// Comparisons
bool equal = (a == b);

// Other operations
int symbol = legendre(a);     // Legendre symbol
sqrt(result, a);              // Square root (NOT constant-time)
```

## Building

```bash
# Build everything (static libraries, tests, and benchmarks)
mkdir build && cd build
cmake ..
make -j4

# Static libraries will be in ../lib/
# Test executables in ./
```

**Run tests:**
```bash
make test              # Run all tests via CMake
./test_p256_0          # Run specific test
```

**Run benchmarks:**
```bash
./bench_p256_0         # Run specific benchmark
```

**Or use the Makefile wrapper:**
```bash
make test    # Build and run tests
make bench   # Build and run benchmarks
```



## License

MIT License - Copyright (c) 2026 Novak Kaluđerović

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
