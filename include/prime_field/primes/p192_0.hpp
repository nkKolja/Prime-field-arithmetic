#pragma once

#include "../params.hpp"

namespace prime_field {

// P192_0: 2^192 - 237
using P192_0 = montgomery::PrimeParameters<0xFFFFFFFFFFFFFF13, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF>;

} // namespace prime_field
