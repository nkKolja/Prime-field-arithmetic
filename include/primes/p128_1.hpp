#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P128_1: 2^128 - 173
struct P128_1 {
    static constexpr size_t NWORDS = 2;
    static constexpr size_t NBITS = 128;
    
    static constexpr std::array<digit_t, NWORDS> p = {0xFFFFFFFFFFFFFF53, 0xFFFFFFFFFFFFFFFF};
    static constexpr std::array<digit_t, NWORDS> Mont_one = {0x00000000000000AD, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> R2 = {0x00000000000074E9, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> iR = {0x882383B30D516318, 0x133CABA736C05EB4};
    static constexpr std::array<digit_t, NWORDS> pp = {0x882383B30D516325, 0x133CABA736C05EB4};
    static constexpr std::array<digit_t, NWORDS> ip = {0x77DC7C4CF2AE9CDB, 0xECC35458C93FA14B};
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {0x00000000000000AD, 0x0000000000000000};
};

} // namespace prime_field
