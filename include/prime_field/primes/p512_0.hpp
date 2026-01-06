#pragma once

#include "../params.hpp"

namespace prime_field {

// P512_0: 2^512 - 2^256 + 2^192 - 2^128 - 1
using P512_0 = montgomery::PrimeParameters<
    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0x0000000000000000,
    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
>;

} // namespace prime_field
