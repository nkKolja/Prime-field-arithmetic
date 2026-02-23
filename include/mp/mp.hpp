#pragma once

// Main header for runtime multi-precision arithmetic
// Include this to get all mp:: functionality

#include "../common/types.hpp"
#include "../common/config.hpp"
#include "primitives.hpp"
#include "comparison.hpp"
#include "logic.hpp"
#include "addition.hpp"
#include "subtraction.hpp"
#include "multiplication.hpp"
#include "division.hpp"

namespace mp {
    // All functions are now available through individual headers
    // This file serves as a convenient single-include entry point
}
