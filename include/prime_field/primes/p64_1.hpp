#pragma once

#include "../params.hpp"

namespace prime_field {

// P64_1: 2^64 - 59
using P64_1 = montgomery::PrimeParameters<0xFFFFFFFFFFFFFFC5>;

} // namespace prime_field
