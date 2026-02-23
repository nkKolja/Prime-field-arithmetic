#pragma once

// Require C++17 minimum
#if __cplusplus < 201703L
    #error "This library requires C++17 or later"
#endif

// Force inline for performance-critical primitives
#if defined(__GNUC__) || defined(__clang__)
    #define MP_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
    #define MP_FORCE_INLINE __forceinline
#else
    #define MP_FORCE_INLINE inline
#endif
