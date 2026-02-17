# Runtime Multi-Precision Arithmetic (MP)

High-performance, runtime multi-precision integer arithmetic for cryptographic operations.

## Note
This file was written with the use of AI. It has not yet been manually checked.

## Purpose

This library provides efficient multi-precision operations optimized for runtime performance. It uses output-parameter style for maximum efficiency and is designed to work with Montgomery arithmetic for prime field operations.

## Headers

- `mp.hpp` - Main header (includes everything)
- `primitives.hpp` - Single-digit operations (addc, subc, mul, div_qr, fdiv_21_qr, fdiv_32_qr, etc.)
- `arithmetic.hpp` - Umbrella header (includes addition, subtraction, multiplication, division)
- `addition.hpp` - Multi-precision addition (mp_add, mp_addc, mp_add_and_divide)
- `subtraction.hpp` - Multi-precision subtraction (mp_sub, mp_subc, mp_sub_conditional)
- `multiplication.hpp` - Multi-precision multiplication (mp_mul, mp_mul_high, mp_mul_digit)
- `division.hpp` - Multi-precision division (mp_div_qr, mp_div_digit_qr, mp_fdiv_21_qr, mp_div_21_qr)
- `logic.hpp` - Logical operations (lshift, rshift, copy, zero)
- `comparison.hpp` - Comparison operations (mp_compare, mp_is_zero, mp_equal)

## Usage

```cpp
#include "mp/mp.hpp"

using namespace common;

std::array<digit_t, 4> a = {1, 2, 3, 4};
std::array<digit_t, 4> b = {5, 6, 7, 8};
std::array<digit_t, 4> result;

// Multi-precision addition
mp::mp_add(result, a, b);

// Multi-precision multiplication
std::array<digit_t, 8> product;
mp::mp_mul(product, a, b);

// Multi-precision division (non-constant-time)
std::array<digit_t, 2> quotient;
std::array<digit_t, 2> remainder;
std::array<digit_t, 4> dividend = {10, 20, 30, 40};
std::array<digit_t, 2> divisor = {3, 4};
mp::mp_div_qr(quotient, remainder, dividend, divisor);

// Comparison
int cmp = mp::mp_compare(a, b); // returns -1, 0, or 1
```

## Design

- **Output parameters** - For zero-copy efficiency
- **Force-inlined** - Maximum performance
- **Template-based** - Compile-time size optimization
- **Montgomery operations** - Constant-time where needed

## Important Notes

- **Division is NOT constant-time** - Division operations (mp_div_qr, mp_div_digit_qr, etc.) are optimized for performance and are NOT suitable for cryptographic contexts where timing attacks are a concern
- **Little-endian word order** - Multi-precision numbers are stored with least significant word first
- **64-bit digits** - All operations use 64-bit words (digit_t = uint64_t)

## Namespace

All functions are in the `mp::` namespace (Multi-Precision runtime).
