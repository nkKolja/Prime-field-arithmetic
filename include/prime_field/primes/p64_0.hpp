#pragma once

#include "../params.hpp"

namespace prime_field {

// P64_0: 2^61 - 1
using P64_0 = montgomery::PrimeParameters<0x1FFFFFFFFFFFFFFF>;

} // namespace prime_field
