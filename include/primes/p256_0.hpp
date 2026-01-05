#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P256_0: 2^255 - 19
struct P256_0 {
    static constexpr size_t NWORDS = 4;
    static constexpr size_t NBITS = 255;
    
    static constexpr std::array<digit_t, NWORDS> p = {
        0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF
    };
    static constexpr std::array<digit_t, NWORDS> Mont_one = {
        0x0000000000000026, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    };
    static constexpr std::array<digit_t, NWORDS> R2 = {
        0x00000000000005A4, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    };
    static constexpr std::array<digit_t, NWORDS> iR = {
        0x435E50D79435E50A, 0x5E50D79435E50D79, 0x50D79435E50D7943, 0x179435E50D79435E
    };
    static constexpr std::array<digit_t, NWORDS> pp = {
        0x86BCA1AF286BCA1B, 0xBCA1AF286BCA1AF2, 0xA1AF286BCA1AF286, 0x2F286BCA1AF286BC
    };
    static constexpr std::array<digit_t, NWORDS> ip = {
        0x79435E50D79435E5, 0x435E50D79435E50D, 0x5E50D79435E50D79, 0xD0D79435E50D7943
    };
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {
        0x0000000000000013, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    };
};

} // namespace prime_field
