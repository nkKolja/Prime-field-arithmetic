#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P128_0: 2^127 - 1 (pseudo-Mersenne prime)
struct P128_0 {
    static constexpr size_t NWORDS = 2;
    static constexpr size_t NBITS = 127;
    
    static constexpr std::array<digit_t, NWORDS> p = {0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF};
    static constexpr std::array<digit_t, NWORDS> Mont_one = {0x0000000000000002, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> R2 = {0x0000000000000004, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> iR = {0x0000000000000000, 0x4000000000000000};
    static constexpr std::array<digit_t, NWORDS> pp = {0x0000000000000001, 0x8000000000000000};
    static constexpr std::array<digit_t, NWORDS> ip = {0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF};
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {0x0000000000000001, 0x0000000000000000};
};

} // namespace prime_field
