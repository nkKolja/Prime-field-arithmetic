#pragma once

#include "../common/types.hpp"
#include "../common/config.hpp"
#include <array>

namespace mp {

using namespace common;

// Three-way comparison: returns 1 if a > b, 0 if equal, -1 if a < b
// TODO: FIX TEMPORARY FUNCTION, MOVE IN FROM LOGIC.HPP
template<size_t N>
MP_FORCE_INLINE constexpr int compare(const std::array<digit_t, N>& a, const std::array<digit_t, N>& b) {
    for (int i = N - 1; i >= 0; i--) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return -1;
    }
    return 0;
}

// Check if value is zero
// TODO: FIX TEMPORARY FUNCTION, MOVE IN FROM LOGIC.HPP
template<size_t N>
MP_FORCE_INLINE constexpr bool is_zero(const std::array<digit_t, N>& a) {
    for (size_t i = 0; i < N; i++) {
        if (a[i] != 0) return false;
    }
    return true;
}

// Check if value is one
// TODO: FIX TEMPORARY FUNCTION, MOVE IN FROM LOGIC.HPP
template<size_t N>
MP_FORCE_INLINE constexpr bool is_one(const std::array<digit_t, N>& a) {
    if (a[0] != 1) return false;
    for (size_t i = 1; i < N; i++) {
        if (a[i] != 0) return false;
    }
    return true;
}

// Equality check
template<size_t N>
MP_FORCE_INLINE constexpr bool equal(const std::array<digit_t, N>& a, const std::array<digit_t, N>& b) {
    for (size_t i = 0; i < N; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

} // namespace mp
