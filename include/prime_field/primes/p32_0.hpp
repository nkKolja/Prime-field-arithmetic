#pragma once

#include "../params.hpp"

namespace prime_field {

// P32_0: 2^31 - 2^27 + 1
using P32_0 = montgomery::PrimeParameters<0x78000001>;

} // namespace prime_field
