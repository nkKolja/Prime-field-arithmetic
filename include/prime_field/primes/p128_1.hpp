#pragma once

#include "../params.hpp"

namespace prime_field {

// P128_1: 2^128 - 173
using P128_1 = montgomery::PrimeParameters<0xFFFFFFFFFFFFFF53, 0xFFFFFFFFFFFFFFFF>;

} // namespace prime_field
