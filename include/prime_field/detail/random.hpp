#pragma once

#include <cstddef>
#include <cerrno>

#if defined(_WIN32)
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
#elif defined(__linux__)
    #include <sys/random.h>
#elif defined(__APPLE__)
    #include <stdlib.h>
#else
    #error "Unsupported platform"
#endif

namespace prime_field {
namespace detail {

inline int randombytes(void *buf, size_t len) {
#if defined(_WIN32)
    if (!BCRYPT_SUCCESS(BCryptGenRandom(NULL, buf, (ULONG)len,
                        BCRYPT_USE_SYSTEM_PREFERRED_RNG)))
        return -1;
    return 0;

#elif defined(__linux__)
    unsigned char *p = static_cast<unsigned char*>(buf);
    while (len) {
        ssize_t r = getrandom(p, len, GRND_NONBLOCK);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        p += r;
        len -= r;
    }
    return 0;

#elif defined(__APPLE__)
    arc4random_buf(buf, len);
    return 0;
#endif
}

} // namespace detail
} // namespace prime_field
