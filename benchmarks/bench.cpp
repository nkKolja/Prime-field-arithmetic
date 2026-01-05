// BENCHMARKING

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include "../include/field_element.hpp"
#include "../include/primes.hpp"

using namespace prime_field;

#define WARMUP      15000
#define BENCH_LOOPS 15000
#define BATCH       1000  // Operations per timing measurement

// Volatile sink prevents compiler from optimizing away function calls
static volatile digit_t sink;

// Returns nanoseconds using monotonic clock (unaffected by system time changes)
static inline uint64_t get_time_ns() {
    struct timespec ts;
#if defined(__linux__)
    // Use MONOTONIC_RAW on Linux (not adjusted by NTP)
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#else
    // Use MONOTONIC on macOS and other platforms
    clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

// Format number with thousands separators for readability
static std::string format_number(uint64_t num) {
    std::string str = std::to_string(num);
    std::string result;
    int count = 0;
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        if (count > 0 && count % 3 == 0) {
            result = '\'' + result;
        }
        result = *it + result;
        count++;
    }
    return result;
}

template<typename Prime>
static void __attribute__((noinline)) bench_fun(int sel, 
    std::vector<FieldElement<Prime>>& t0, 
    std::vector<FieldElement<Prime>>& t1, 
    std::vector<unsigned char>& s, 
    int i) 
{
    switch (sel) {
        case 0: reduce(t0[i]); sink = t0[i].data[0]; break;
        case 1: add(t0[i], t1[i], t0[i]); sink = t0[i].data[0]; break;
        case 2: neg(t0[i], t0[i]); sink = t0[i].data[0]; break;
        case 3: sub(t0[i], t1[i], t0[i]); sink = t0[i].data[0]; break;
        case 4: mul(t0[i], t1[i], t0[i]); sink = t0[i].data[0]; break;
        case 5: s[i] = legendre(t0[i]); sink = s[i]; break;
        case 6: inv(t0[i], t0[i]); sink = t0[i].data[0]; break;
        case 7: sqrt(t0[i], t0[i]); sink = t0[i].data[0]; break;
        default: break;
    }
}

template<typename Prime>
void run_benchmark(const char* prime_name) {
    using F = FieldElement<Prime>;
    
    const char *function_names[] = {"f_red", "f_add", "f_neg", "f_sub", "f_mul", "f_leg", "f_inv", "f_sqrt"};
    
    std::vector<F> t0(BENCH_LOOPS);
    std::vector<F> t1(BENCH_LOOPS);
    std::vector<unsigned char> s(BENCH_LOOPS);

    // Generate random field values
    srand(14); // Fixed seed for reproducibility
    for(int i = 0; i < BENCH_LOOPS; i++) {
        for(size_t j = 0; j < Prime::NWORDS; j++) {
            t0[i].data[j] = ((uint64_t)rand() << 32) | rand();
            t1[i].data[j] = ((uint64_t)rand() << 32) | rand();
        }
        if(Prime::NWORDS * RADIX != Prime::NBITS){
            size_t excess_bits = Prime::NWORDS * RADIX - Prime::NBITS;
            digit_t mask = (digit_t(1) << (RADIX - excess_bits)) - 1;
            t0[i].data[Prime::NWORDS - 1] &= mask;
            t1[i].data[Prime::NWORDS - 1] &= mask;
        }
        reduce(t0[i]);
        reduce(t1[i]);
    }

    // WARMUP
    for(int i = 0; i < WARMUP; i++){
        mul(t0[i], t1[i], t1[i]);
    }

    // BENCHMARKING
    std::cout << "Benchmarking " << prime_name << "\n\n";

    for(int sel = 0; sel < 8; sel++){
        uint64_t total_ns = 0;
        
        for(int i = 0; i < BENCH_LOOPS; i += BATCH){
            int batch_end = (i + BATCH <= BENCH_LOOPS) ? i + BATCH : BENCH_LOOPS;
            uint64_t time_start = get_time_ns();
            for(int j = i; j < batch_end; j++){
                bench_fun(sel, t0, t1, s, j);
            }
            uint64_t time_end = get_time_ns();
            total_ns += (time_end - time_start);
        }
        
        double avg_ns = static_cast<double>(total_ns) / BENCH_LOOPS;
        std::cout << std::left << std::setw(8) << function_names[sel] 
                  << " " << std::right << std::setw(9) << format_number(static_cast<uint64_t>(avg_ns)) 
                  << " ns/op\n";
    }
    std::cout << "\n";
}

int main() {
    const char* prime_names[] = {"p64_0", "p64_1", "p128_0", "p128_1", "p192_0", "p192_1", "p256_0", "p256_1", "p512_0", "p512_1"};
    std::cout << "======================\n";
    std::cout << "Benchmarking " << prime_names[PRIME_ID] << "\n";
    std::cout << "======================\n";
    run_benchmark<Prime>(prime_names[PRIME_ID]);

    return 0;
}
