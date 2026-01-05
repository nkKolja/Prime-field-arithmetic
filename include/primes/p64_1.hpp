#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P64_1: 2^64 - 59
struct P64_1 {
    static constexpr size_t NWORDS = 1;
    static constexpr size_t NBITS = 64;
    
    static constexpr std::array<digit_t, NWORDS> p = {0xFFFFFFFFFFFFFFC5};
    static constexpr std::array<digit_t, NWORDS> Mont_one = {0x000000000000003B};
    static constexpr std::array<digit_t, NWORDS> R2 = {0x0000000000000D99};
    static constexpr std::array<digit_t, NWORDS> iR = {0xCBEEA4E1A08AD8C4};
    static constexpr std::array<digit_t, NWORDS> pp = {0xCBEEA4E1A08AD8F3};
    static constexpr std::array<digit_t, NWORDS> ip = {0x34115B1E5F75270D};
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {0x000000000000003b};
};

} // namespace prime_field
