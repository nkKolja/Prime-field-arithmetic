#pragma once

#include "../params.hpp"

namespace prime_field {

// P512_1: 2^511 - 2^320 + 1
using P512_1 = montgomery::PrimeParameters<
    0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF
>;

} // namespace prime_field
