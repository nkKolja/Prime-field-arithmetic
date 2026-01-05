#pragma once

#include "../types.hpp"
#include <array>

namespace prime_field {

// P64_0: 2^61 - 1
struct P64_0 {
    static constexpr size_t NWORDS = 1;
    static constexpr size_t NBITS = 61;
    
    static constexpr std::array<digit_t, NWORDS> p = {0x1FFFFFFFFFFFFFFF};
    static constexpr std::array<digit_t, NWORDS> Mont_one = {0x0000000000000008};
    static constexpr std::array<digit_t, NWORDS> R2 = {0x0000000000000040};
    static constexpr std::array<digit_t, NWORDS> pp = {0x2000000000000001};
    static constexpr std::array<digit_t, NWORDS> iR = {0x4000000000000000};
    static constexpr std::array<digit_t, NWORDS> ip = {0xDFFFFFFFFFFFFFFF};
    static constexpr std::array<digit_t, NWORDS> TWO_POW_NBITS = {0x0000000000000001};
};

} // namespace prime_field
