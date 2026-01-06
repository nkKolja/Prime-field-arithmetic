#pragma once

#include "../params.hpp"

namespace prime_field {

// P256_0: 2^255 - 19
using P256_0 = montgomery::PrimeParameters<
    0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF
>;

} // namespace prime_field
