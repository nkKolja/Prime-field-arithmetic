#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P192_1: 2^191 - 19
struct P192_1 {
    static constexpr size_t NWORDS = 3;
    static constexpr size_t NBITS = 191;
    
    static constexpr std::array<digit_t, NWORDS> p = {0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF};
    static constexpr std::array<digit_t, NWORDS> Mont_one = {0x0000000000000026, 0x0000000000000000, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> R2 = {0x00000000000005A4, 0x0000000000000000, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> iR = {0x435E50D79435E50B, 0x5E50D79435E50D79, 0x10D79435E50D7943};
    static constexpr std::array<digit_t, NWORDS> pp = {0x86BCA1AF286BCA1B, 0xBCA1AF286BCA1AF2, 0x21AF286BCA1AF286};
    static constexpr std::array<digit_t, NWORDS> ip = {0x79435E50D79435E5, 0x435E50D79435E50D, 0xDE50D79435E50D79};
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {0x0000000000000013, 0x0000000000000000, 0x0000000000000000};
};

} // namespace prime_field
