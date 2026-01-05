#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P256_1: 2^256 - 2^224 + 2^192 + 2^96 - 1
struct P256_1 {
    static constexpr size_t NWORDS = 4;
    static constexpr size_t NBITS = 256;
    
    static constexpr std::array<digit_t, NWORDS> p = {
        0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFF, 0x0000000000000000, 0xFFFFFFFF00000001
    };
    static constexpr std::array<digit_t, NWORDS> Mont_one = {
        0x0000000000000001, 0xFFFFFFFF00000000, 0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFE
    };
    static constexpr std::array<digit_t, NWORDS> R2 = {
        0x0000000000000003, 0xFFFFFFFBFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0x00000004FFFFFFFD
    };
    static constexpr std::array<digit_t, NWORDS> iR = {
        0x0000000300000000, 0x00000001FFFFFFFE, 0xFFFFFFFD00000002, 0xFFFFFFFE00000003
    };
    static constexpr std::array<digit_t, NWORDS> pp = {
        0x0000000000000001, 0x0000000100000000, 0x0000000000000000, 0xFFFFFFFF00000002
    };
    static constexpr std::array<digit_t, NWORDS> ip = {
        0xFFFFFFFFFFFFFFFF, 0xFFFFFFFEFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFD
    };
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {
        0x0000000000000001, 0xFFFFFFFF00000000, 0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFE
    };
};

} // namespace prime_field
