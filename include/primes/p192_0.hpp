#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P192_0: 2^192 - 237
struct P192_0 {
    static constexpr size_t NWORDS = 3;
    static constexpr size_t NBITS = 192;
    
    static constexpr std::array<digit_t, NWORDS> p = {0xFFFFFFFFFFFFFF13, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
    static constexpr std::array<digit_t, NWORDS> Mont_one = {0x00000000000000ED, 0x0000000000000000, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> R2 = {0x000000000000DB69, 0x0000000000000000, 0x0000000000000000};
    static constexpr std::array<digit_t, NWORDS> iR = {0xDE83C7D4CB125C9E, 0xF1F532C497393FBA, 0x4CB125CE4FEEB7A0};
    static constexpr std::array<digit_t, NWORDS> pp = {0xDE83C7D4CB125CE5, 0xF1F532C497393FBA, 0x4CB125CE4FEEB7A0};
    static constexpr std::array<digit_t, NWORDS> ip = {0x217C382B34EDA31B, 0x0E0ACD3B68C6C045, 0xB34EDA31B011485F};
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {0x00000000000000ED, 0x0000000000000000, 0x0000000000000000};
};

} // namespace prime_field
