/**
 * @file prime_field.hpp
 * @brief Main header for the Prime Field Arithmetic library
 * 
 * This header provides a complete interface to the Prime Field Arithmetic library,
 * a high-performance C++ template library for prime field arithmetic using
 * Montgomery form for efficient modular multiplication.
 * 
 * @section usage Basic Usage
 * @code
 * #include <prime_field.hpp>
 * 
 * using namespace prime_field;
 * 
 * // Use a predefined prime field
 * using Field = FieldElement<P256_0>;
 * 
 * Field a(42);
 * Field b(17);
 * Field c = a + b;  // Modular addition
 * Field d = a * b;  // Montgomery multiplication
 * @endcode
 * 
 * @section features Features
 * - Header-only template library (zero runtime dependencies)
 * - Compile-time Montgomery parameter computation
 * - Constant-time operations for cryptographic security
 * - Support for custom prime fields
 * - Optimized multi-precision arithmetic
 * - 10 pre-configured prime fields (64 to 512 bits)
 * 
 * @section performance Performance
 * All operations are optimized using:
 * - Montgomery form for efficient modular multiplication
 * - Template metaprogramming for compile-time optimization
 * - Multi-precision arithmetic with carry handling
 * - Constant-time implementations where security-critical
 * 
 * @author Kolja
 * @copyright MIT License
 */

#pragma once

// Core types
#include "prime_field/types.hpp"

// Montgomery parameter computation
#include "prime_field/params.hpp"

// Field element implementation
#include "prime_field/field_element.hpp"

// Predefined prime fields
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

/**
 * @namespace prime_field
 * @brief Main namespace for prime field arithmetic operations
 * 
 * This namespace contains all public API elements including:
 * - FieldElement<Prime>: Template class for field elements
 * - Predefined prime types (P64_0, P128_0, etc.)
 * - Field operations (add, sub, mul, div, inv, pow, sqrt, legendre)
 * - Montgomery form conversions
 */

/**
 * @namespace prime_field::montgomery
 * @brief Montgomery arithmetic parameter computation
 * 
 * Contains compile-time functions for computing Montgomery parameters
 * and the PrimeParameters template for defining new prime fields.
 */
