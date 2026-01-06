#pragma once

#include "../params.hpp"

namespace prime_field {

// P256_1: 2^256 - 2^224 + 2^192 + 2^96 - 1
using P256_1 = montgomery::PrimeParameters<
    0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFF, 0x0000000000000000, 0xFFFFFFFF00000001
>;

} // namespace prime_field
