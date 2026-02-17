#pragma once

#include "../common/primitives.hpp"

namespace mp {

// Import primitives from common namespace
using common::addc;
using common::add;
using common::subc;
using common::sub;
using common::mul;
using common::mul_hi;
using common::mul_lo;
using common::div_qr;
using common::div_q;
using common::div_r;
using common::div_2qr;
using common::div_2q;
using common::apx_21;
using common::fdiv_21_qr;
using common::apx_32;
using common::fdiv_32_qr;

} // namespace mp
