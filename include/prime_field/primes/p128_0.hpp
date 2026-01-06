#pragma once

#include "../params.hpp"

namespace prime_field {

// P128_0: 2^127 - 1
using P128_0 = montgomery::PrimeParameters<0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF>;

} // namespace prime_field
