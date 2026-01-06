#pragma once

#include "../params.hpp"

namespace prime_field {

// P192_1: 2^191 - 19
using P192_1 = montgomery::PrimeParameters<0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF>;

} // namespace prime_field
