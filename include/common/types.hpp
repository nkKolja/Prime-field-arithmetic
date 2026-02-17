#pragma once

#include <cstdint>
#include <array>

namespace common {

// Multi-precision arithmetic traits
template<typename Digit>
struct mp_traits;

// Specialization for uint64_t
template<>
struct mp_traits<uint64_t> {
    using digit_t = uint64_t;
    using digit2_t = __uint128_t;
    static constexpr size_t RADIX = 64;
    static constexpr digit_t ONE = 1;
    static constexpr digit_t MAX_DIGIT = ~static_cast<digit_t>(0);
    static constexpr digit2_t MAX_DIGIT2 = ~static_cast<digit2_t>(0);
};

// Specialization for uint32_t
template<>
struct mp_traits<uint32_t> {
    using digit_t = uint32_t;
    using digit2_t = uint64_t;
    static constexpr size_t RADIX = 32;
    static constexpr digit_t ONE = 1;
    static constexpr digit_t MAX_DIGIT = ~static_cast<digit_t>(0);
    static constexpr digit2_t MAX_DIGIT2 = ~static_cast<digit2_t>(0);
};

// Default configuration (can be changed with #define before including)
#ifndef MP_DIGIT_TYPE
#define MP_DIGIT_TYPE uint64_t
#endif

// Export default types
using default_traits = mp_traits<MP_DIGIT_TYPE>;
using digit_t = typename default_traits::digit_t;
using digit2_t = typename default_traits::digit2_t;
constexpr size_t RADIX = default_traits::RADIX;
constexpr digit_t ONE = default_traits::ONE;
constexpr digit_t MAX_DIGIT = default_traits::MAX_DIGIT;
constexpr digit2_t MAX_DIGIT2 = default_traits::MAX_DIGIT2;

} // namespace common
