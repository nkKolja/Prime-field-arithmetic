#pragma once

#include <cstdint>

namespace prime_field {

#define RADIX 64
using digit_t = uint64_t;
using uint128_t = unsigned __int128;

// Basic arithmetic macros
#define MUL(_hi, _lo, _in1, _in2) {                                                       \
    uint128_t t_mul = (uint128_t)(_in1) * (uint128_t)(_in2);                              \
    (_hi) = (uint64_t)(t_mul >> (sizeof(uint64_t) * 8));                                  \
    (_lo) = (uint64_t)t_mul;                                                              \
}

#define ADDC(_out, _in1, _in2, _carry) {                                                  \
    uint128_t t_addc = (uint128_t)(_in1) + (uint128_t)(_in2) + (uint128_t)(_carry);       \
    (_carry) = (uint64_t)(t_addc >> (sizeof(uint64_t) * 8));                              \
    (_out) = (uint64_t)t_addc;                                                            \
}

#define SUBC(_out, _in1, _in2, _borrow) {                                                 \
    uint128_t _t_subc = (uint128_t)(_in1) - (uint128_t)(_in2) - (uint128_t)(_borrow);     \
    (_borrow) = (uint64_t)(_t_subc >> (sizeof(uint128_t) * 8 - 1));                       \
    (_out) = (uint64_t)_t_subc;                                                           \
}

// Assumes _den != 0
#define DIVR_DBL(_quo_hi, _quo_lo, _rem, _num_hi, _num_lo, _den) {                       \
    uint128_t _n = ((uint128_t)(_num_hi) << 64) | (uint128_t)(_num_lo);                  \
    uint128_t _quo = _n / (uint128_t)(_den);                                             \
    (_quo_hi) = (uint64_t)(_quo >> 64);                                                  \
    (_quo_lo) = (uint64_t)(_quo);                                                        \
    (_rem) = (uint64_t)(_n % (uint128_t)(_den));                                         \
}

// Assumes _den != 0
// Assumes _num / _den fits in 64 bits
#define DIVR(_quo, _rem, _num_hi, _num_lo, _den) {                                       \
    uint128_t _n = ((uint128_t)(_num_hi) << 64) | (uint128_t)(_num_lo);                  \
    uint128_t _quo = (uint64_t)(_n / (uint128_t)(_den));                                 \
    (_rem) = (uint64_t)(_n % (uint128_t)(_den));                                         \
}
} // namespace prime_field
