// BENCHMARKING DIVISION OPERATIONS

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include "../include/mp/arithmetic.hpp"
#include "../include/common/types.hpp"

using namespace mp;
using namespace common;

#define WARMUP      5000
#define BENCH_LOOPS 100000
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
    for (auto it = str.rbegin(); it != str.rend(); it++) {
        if (count > 0 && count % 3 == 0) {
            result = '\'' + result;
        }
        result = *it + result;
        count++;
    }
    return result;
}

template<size_t N>
static void __attribute__((noinline)) bench_div_q(
    std::array<digit_t, N>& quo,
    const std::array<digit_t, N>& num,
    digit_t divisor)
{
    mp_div_digit_q<N>(quo, num, divisor);
    sink = quo[0];
}

template<size_t N>
static void __attribute__((noinline)) bench_div_qr(
    std::array<digit_t, N>& quo,
    digit_t& rem,
    const std::array<digit_t, N>& num,
    digit_t divisor)
{
    mp_div_digit_qr<N>(quo, rem, num, divisor);
    sink = quo[0] ^ rem;
}

template<size_t N>
static void __attribute__((noinline)) bench_div_r(
    digit_t& rem,
    const std::array<digit_t, N>& num,
    digit_t divisor)
{
    mp_div_digit_r<N>(rem, num, divisor);
    sink = rem;
}

template<size_t N>
static void __attribute__((noinline)) bench_fdiv_q(
    std::array<digit_t, N>& quo,
    const std::array<digit_t, N>& num,
    digit_t divisor)
{
    mp_fdiv_digit_q<N>(quo, num, divisor);
    sink = quo[0];
}

template<size_t N>
static void __attribute__((noinline)) bench_fdiv_qr(
    std::array<digit_t, N>& quo,
    digit_t& rem,
    const std::array<digit_t, N>& num,
    digit_t divisor)
{
    mp_fdiv_digit_qr<N>(quo, rem, num, divisor);
    sink = quo[0] ^ rem;
}

template<size_t M, size_t N>
static void __attribute__((noinline)) bench_mp_div_qr(
    std::array<digit_t, M>& quo,
    std::array<digit_t, N>& rem,
    const std::array<digit_t, M>& num,
    const std::array<digit_t, N>& den)
{
    mp_div_qr<M, N, M>(quo, rem, num, den);
    sink = quo[0] ^ rem[0];
}

template<size_t N>
void run_benchmark(const char* size_name) {
    std::vector<std::array<digit_t, N>> nums(BENCH_LOOPS);
    std::vector<digit_t> divisors(BENCH_LOOPS);
    std::array<digit_t, N> quo;
    digit_t rem;
    
    // Generate random inputs
    srand(42); // Fixed seed for reproducibility
    for (int i = 0; i < BENCH_LOOPS; i++) {
        for (size_t j = 0; j < N; j++) {
            nums[i][j] = ((uint64_t)rand() << 32) | rand();
        }
        divisors[i] = ((uint64_t)rand() << 32) | rand();
        if (divisors[i] == 0) divisors[i] = 1; // Avoid division by zero
    }
    
    // WARMUP
    for (int i = 0; i < WARMUP; i++) {
        mp_div_digit_qr<N>(quo, rem, nums[i], divisors[i]);
    }
    
    // BENCHMARKING
    std::cout << "Benchmarking " << size_name << "\n\n";
    
    const char* function_names[] = {"div_q", "div_qr", "div_r", "fdiv_q", "fdiv_qr"};
    
    // Benchmark div_q
    {
        uint64_t total_ns = 0;
        for (int i = 0; i < BENCH_LOOPS; i += BATCH) {
            int batch_end = (i + BATCH <= BENCH_LOOPS) ? i + BATCH : BENCH_LOOPS;
            uint64_t time_start = get_time_ns();
            for (int j = i; j < batch_end; j++) {
                bench_div_q<N>(quo, nums[j], divisors[j]);
            }
            uint64_t time_end = get_time_ns();
            total_ns += (time_end - time_start);
        }
        double avg_ns = static_cast<double>(total_ns) / BENCH_LOOPS;
        std::cout << std::left << std::setw(8) << function_names[0]
                  << " " << std::right << std::setw(9) << format_number(static_cast<uint64_t>(avg_ns))
                  << " ns/op\n";
    }
    
    // Benchmark div_qr
    {
        uint64_t total_ns = 0;
        for (int i = 0; i < BENCH_LOOPS; i += BATCH) {
            int batch_end = (i + BATCH <= BENCH_LOOPS) ? i + BATCH : BENCH_LOOPS;
            uint64_t time_start = get_time_ns();
            for (int j = i; j < batch_end; j++) {
                bench_div_qr<N>(quo, rem, nums[j], divisors[j]);
            }
            uint64_t time_end = get_time_ns();
            total_ns += (time_end - time_start);
        }
        double avg_ns = static_cast<double>(total_ns) / BENCH_LOOPS;
        std::cout << std::left << std::setw(8) << function_names[1]
                  << " " << std::right << std::setw(9) << format_number(static_cast<uint64_t>(avg_ns))
                  << " ns/op\n";
    }
    
    // Benchmark div_r
    {
        uint64_t total_ns = 0;
        for (int i = 0; i < BENCH_LOOPS; i += BATCH) {
            int batch_end = (i + BATCH <= BENCH_LOOPS) ? i + BATCH : BENCH_LOOPS;
            uint64_t time_start = get_time_ns();
            for (int j = i; j < batch_end; j++) {
                bench_div_r<N>(rem, nums[j], divisors[j]);
            }
            uint64_t time_end = get_time_ns();
            total_ns += (time_end - time_start);
        }
        double avg_ns = static_cast<double>(total_ns) / BENCH_LOOPS;
        std::cout << std::left << std::setw(8) << function_names[2]
                  << " " << std::right << std::setw(9) << format_number(static_cast<uint64_t>(avg_ns))
                  << " ns/op\n";
    }
    
    // Benchmark fdiv_q (fast division)
    {
        uint64_t total_ns = 0;
        for (int i = 0; i < BENCH_LOOPS; i += BATCH) {
            int batch_end = (i + BATCH <= BENCH_LOOPS) ? i + BATCH : BENCH_LOOPS;
            uint64_t time_start = get_time_ns();
            for (int j = i; j < batch_end; j++) {
                bench_fdiv_q<N>(quo, nums[j], divisors[j]);
            }
            uint64_t time_end = get_time_ns();
            total_ns += (time_end - time_start);
        }
        double avg_ns = static_cast<double>(total_ns) / BENCH_LOOPS;
        std::cout << std::left << std::setw(8) << function_names[3]
                  << " " << std::right << std::setw(9) << format_number(static_cast<uint64_t>(avg_ns))
                  << " ns/op\n";
    }
    
    // Benchmark fdiv_qr (fast division with remainder)
    {
        uint64_t total_ns = 0;
        for (int i = 0; i < BENCH_LOOPS; i += BATCH) {
            int batch_end = (i + BATCH <= BENCH_LOOPS) ? i + BATCH : BENCH_LOOPS;
            uint64_t time_start = get_time_ns();
            for (int j = i; j < batch_end; j++) {
                bench_fdiv_qr<N>(quo, rem, nums[j], divisors[j]);
            }
            uint64_t time_end = get_time_ns();
            total_ns += (time_end - time_start);
        }
        double avg_ns = static_cast<double>(total_ns) / BENCH_LOOPS;
        std::cout << std::left << std::setw(8) << function_names[4]
                  << " " << std::right << std::setw(9) << format_number(static_cast<uint64_t>(avg_ns))
                  << " ns/op\n";
    }
    
    std::cout << "\n";
}

template<size_t M, size_t N>
void run_mp_div_qr_benchmark(const char* size_name) {
    constexpr size_t K = M;
    std::vector<std::array<digit_t, M>> nums(BENCH_LOOPS);
    std::vector<std::array<digit_t, N>> dens(BENCH_LOOPS);
    std::array<digit_t, K> quo;
    std::array<digit_t, N> rem;
    
    // Generate random inputs
    srand(789); // Different seed for mp_div_qr benchmarks
    for (int i = 0; i < BENCH_LOOPS; i++) {
        for (size_t j = 0; j < M; j++) {
            nums[i][j] = ((uint64_t)rand() << 32) | rand();
        }
        for (size_t j = 0; j < N; j++) {
            dens[i][j] = ((uint64_t)rand() << 32) | rand();
        }
        // Ensure highest word is non-zero
        if (dens[i][N-1] == 0) dens[i][N-1] = 1;
    }
    
    // WARMUP
    for (int i = 0; i < WARMUP && i < BENCH_LOOPS; i++) {
        mp_div_qr<K, N, M>(quo, rem, nums[i], dens[i]);
    }
    
    // BENCHMARKING
    uint64_t total_ns = 0;
    for (int i = 0; i < BENCH_LOOPS; i += BATCH) {
        int batch_end = (i + BATCH <= BENCH_LOOPS) ? i + BATCH : BENCH_LOOPS;
        uint64_t time_start = get_time_ns();
        for (int j = i; j < batch_end; j++) {
            bench_mp_div_qr<M, N>(quo, rem, nums[j], dens[j]);
        }
        uint64_t time_end = get_time_ns();
        total_ns += (time_end - time_start);
    }
    
    double avg_ns = static_cast<double>(total_ns) / BENCH_LOOPS;
    std::cout << std::left << std::setw(12) << size_name
              << " " << std::right << std::setw(12) << format_number(static_cast<uint64_t>(avg_ns))
              << " ns/op\n";
}

template<size_t M, size_t N>
bool test_mp_div_qr_correctness(const char* size_name, int num_tests = 1000) {
    constexpr size_t K = M; // Quotient size (can be M-N+1, but M is safe)
    std::array<digit_t, M> num;
    std::array<digit_t, N> den, rem;
    std::array<digit_t, K> quo;
    std::array<digit_t, M + N> temp;
    bool all_pass = true;
    int fails = 0;
    
    srand(456); // Different seed for mp_div_qr tests
    
    for (int t = 0; t < num_tests; t++) {
        // Generate random numerator
        for (size_t i = 0; i < M; i++) {
            num[i] = ((uint64_t)rand() << 32) | rand();
        }
        
        // Generate random denominator (ensure it's non-zero and normalized)
        for (size_t i = 0; i < N; i++) {
            den[i] = ((uint64_t)rand() << 32) | rand();
        }
        // Ensure highest word is non-zero
        if (den[N-1] == 0) den[N-1] = 1;
        
        // Perform division
        mp_div_qr<K, N, M>(quo, rem, num, den);
        
        // Test 1: Verify reconstruction property: quo * den + rem == num
        mp_mul<M + N, K, N>(temp, quo, den);
        
        digit_t carry = 0;
        bool reconstruction_ok = true;
        for (size_t i = 0; i < N; i++) {
            digit_t sum_lo, sum_hi;
            addc(sum_lo, carry, temp[i], rem[i]);
            if (sum_lo != num[i]) {
                reconstruction_ok = false;
                break;
            }
        }
        // Check remaining words of num match temp
        for (size_t i = N; i < M; i++) {
            digit_t expected;
            addc(expected, carry, temp[i], 0);
            if (expected != num[i]) {
                reconstruction_ok = false;
                break;
            }
        }
        
        if (!reconstruction_ok) {
            std::cout << "FAIL [" << size_name << "]: quo * den + rem != num\n";
            std::cout << "  num = [";
            for(size_t i = 0; i < M; i++)
                std::cout << std::hex << num[i] << (i < M-1 ? " " : "");
            std::cout << std::dec << "]\n";
            std::cout << "  den = [";
            for(size_t i = 0; i < N; i++)
                std::cout << std::hex << den[i] << (i < N-1 ? " " : "");
            std::cout << std::dec << "]\n";
            std::cout << "  quo = [";
            for(size_t i = 0; i < K; i++)
                std::cout << std::hex << quo[i] << (i < K-1 ? " " : "");
            std::cout << std::dec << "]\n";
            std::cout << "  rem = [";
            for(size_t i = 0; i < N; i++)
                std::cout << std::hex << rem[i] << (i < N-1 ? " " : "");
            std::cout << std::dec << "]\n";
            return false;
        }
        
        // Test 2: Verify remainder bounds: rem < den
        bool rem_ok = true;
        for (int i = N - 1; i >= 0; i--) {
            if (rem[i] < den[i]) {
                break;
            } else if (rem[i] > den[i]) {
                rem_ok = false;
                break;
            }
        }
        
        if (!rem_ok) {
            std::cout << "FAIL [" << size_name << "]: rem >= den\n";
            return false;
        }
    }
    
    if (all_pass) {
        std::cout << "PASS [" << size_name << "]: " << num_tests << " tests\n";
    } else {
        std::cout << "FAIL [" << size_name << "]: " << fails << "/" << num_tests << " failed\n";
    }
    
    return all_pass;
}

bool test_mp_div_qr_edge_cases() {
    constexpr size_t M = 3;
    constexpr size_t N = 2;
    constexpr size_t K = M;
    
    // Define test numerators (3 words)
    std::array<digit_t, M> nums[4] = {
        {0, 0, 1ULL << (RADIX - 1)},
        {0, MAX_DIGIT, 1ULL << (RADIX - 1)},
        {MAX_DIGIT, MAX_DIGIT, 1ULL << (RADIX - 1)},
        {MAX_DIGIT, MAX_DIGIT, MAX_DIGIT}
    };
    
    // Define test denominators (2 words)
    std::array<digit_t, N> dens[3] = {
        {0, 1ULL << (RADIX - 1)},
        {MAX_DIGIT, 1ULL << (RADIX - 1)},
        {MAX_DIGIT, MAX_DIGIT}
    };
    
    const char* num_names[4] = {
        "[0, 0, 1<<(R-1)]",
        "[0, MAX, 1<<(R-1)]",
        "[MAX, MAX, 1<<(R-1)]",
        "[MAX, MAX, MAX]"
    };
    
    const char* den_names[3] = {
        "[0, 1<<(R-1)]",
        "[MAX, 1<<(R-1)]",
        "[MAX, MAX]"
    };
    
    bool all_pass = true;
    int test_num = 0;
    
    std::cout << "\nEdge case tests (3/2 words):\n";
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            test_num++;
            std::array<digit_t, K> quo;
            std::array<digit_t, N> rem;
            std::array<digit_t, M + N> temp;
            
            // Perform division
            mp_div_qr<K, N, M>(quo, rem, nums[i], dens[j]);
            
            // Verify reconstruction: quo * den + rem == num
            mp_mul<M + N, K, N>(temp, quo, dens[j]);
            
            digit_t carry = 0;
            bool reconstruction_ok = true;
            for (size_t k = 0; k < N; k++) {
                digit_t sum_lo;
                addc(sum_lo, carry, temp[k], rem[k]);
                if (sum_lo != nums[i][k]) {
                    reconstruction_ok = false;
                    break;
                }
            }
            for (size_t k = N; k < M; k++) {
                digit_t expected;
                addc(expected, carry, temp[k], 0);
                if (expected != nums[i][k]) {
                    reconstruction_ok = false;
                    break;
                }
            }
            
            // Verify remainder bounds: rem < den
            bool rem_ok = true;
            for (int k = N - 1; k >= 0; k--) {
                if (rem[k] < dens[j][k]) {
                    break;
                } else if (rem[k] > dens[j][k]) {
                    rem_ok = false;
                    break;
                }
            }
            
            if (!reconstruction_ok || !rem_ok) {
                std::cout << "FAIL Test " << test_num << ": " << num_names[i] << " / " << den_names[j] << "\n";
                std::cout << "  num = [";
                for(size_t k = 0; k < M; k++)
                    std::cout << std::hex << nums[i][k] << (k < M-1 ? " " : "");
                std::cout << std::dec << "]\n";
                std::cout << "  den = [";
                for(size_t k = 0; k < N; k++)
                    std::cout << std::hex << dens[j][k] << (k < N-1 ? " " : "");
                std::cout << std::dec << "]\n";
                std::cout << "  quo = [";
                for(size_t k = 0; k < K; k++)
                    std::cout << std::hex << quo[k] << (k < K-1 ? " " : "");
                std::cout << std::dec << "]\n";
                std::cout << "  rem = [";
                for(size_t k = 0; k < N; k++)
                    std::cout << std::hex << rem[k] << (k < N-1 ? " " : "");
                std::cout << std::dec << "]\n";
                if (!reconstruction_ok) std::cout << "  Error: quo * den + rem != num\n";
                if (!rem_ok) std::cout << "  Error: rem >= den\n";
                all_pass = false;
            }
        }
    }
    
    if (all_pass) {
        std::cout << "PASS: All " << test_num << " edge case tests passed\n";
    } else {
        std::cout << "FAIL: Some edge case tests failed\n";
    }
    
    return all_pass;
}

template<size_t N>
bool test_correctness(const char* size_name, int num_tests = 10000) {
    std::array<digit_t, N> num, quo1, quo2;
    std::array<digit_t, N + 1> temp;
    digit_t divisor, rem1, rem2;
    bool all_pass = true;
    int fails = 0;
    
    srand(123); // Different seed for tests
    
    for (int t = 0; t < num_tests; t++) {
        // Generate random input
        for (size_t i = 0; i < N; i++) {
            num[i] = ((uint64_t)rand() << 32) | rand();
        }
        divisor = ((uint64_t)rand() << 32) | rand();
        if (divisor == 0) divisor = 1;
        
        // Test 1: div_q and fdiv_q produce same quotient
        mp_div_digit_q<N>(quo1, num, divisor);
        mp_fdiv_digit_q<N>(quo2, num, divisor);
        
        bool quo_match = true;
        for (size_t i = 0; i < N; i++) {
            if (quo1[i] != quo2[i]) {
                quo_match = false;
                break;
            }
        }
        
        if (!quo_match) {
            if (fails == 0) {
                std::cout << "FAIL [" << size_name << "]: div_q != fdiv_q\n";
                std::cout << "  num = ["; 
                for(size_t i=0; i<N; i++) 
                    std::cout << std::hex << num[i] << (i < N-1 ? " " : ""); 
                std::cout << std::dec << "]\n";
                std::cout << "  div = " << std::hex << divisor << std::dec << "\n";
                std::cout << "  div_q  = [";
                for(size_t i=0; i<N; i++)
                    std::cout << std::hex << quo1[i] << (i < N-1 ? " " : "");
                std::cout << std::dec << "]\n";
                std::cout << "  fdiv_q = [";
                for(size_t i=0; i<N; i++)
                    std::cout << std::hex << quo2[i] << (i < N-1 ? " " : "");
                std::cout << std::dec << "]\n";
            }
            all_pass = false;
            fails++;
        }
        
        // Test 2: div_qr produces correct quotient and remainder
        mp_div_digit_qr<N>(quo1, rem1, num, divisor);
        
        // Check: quo * divisor + rem == num (in the low N words)
        mp_mul_digit<N>(temp, quo1, divisor);
        digit_t carry = rem1;
        bool reconstruction_ok = true;
        for (size_t i = 0; i < N; i++) {
            digit2_t sum = (digit2_t)temp[i] + carry;
            if ((digit_t)sum != num[i]) {
                reconstruction_ok = false;
                break;
            }
            carry = sum >> RADIX;
        }
        // Also check that high word + final carry is zero (no overflow)
        if (temp[N] + carry != 0) {
            reconstruction_ok = false;
        }
        
        if (!reconstruction_ok || rem1 >= divisor) {
            if (fails == 0) {
                std::cout << "FAIL [" << size_name << "]: div_qr incorrect (quo*div+rem != num or rem >= div)\n";
            }
            all_pass = false;
            fails++;
        }
        
        // Test 3: div_r produces same remainder as div_qr
        mp_div_digit_r<N>(rem2, num, divisor);
        
        if (rem1 != rem2) {
            if (fails == 0) {
                std::cout << "FAIL [" << size_name << "]: div_r != div_qr remainder\n";
            }
            all_pass = false;
            fails++;
        }
        
        // Test 4: fdiv_qr produces same results as div_qr
        mp_fdiv_digit_qr<N>(quo2, rem2, num, divisor);
        
        bool fdiv_qr_match = true;
        for (size_t i = 0; i < N; i++) {
            if (quo1[i] != quo2[i]) {
                fdiv_qr_match = false;
                break;
            }
        }
        
        if (!fdiv_qr_match || rem1 != rem2) {
            if (fails == 0) {
                std::cout << "FAIL [" << size_name << "]: fdiv_qr != div_qr\n";
                std::cout << "  num = ["; 
                for(size_t i=0; i<N; i++) 
                    std::cout << std::hex << num[i] << (i < N-1 ? " " : ""); 
                std::cout << std::dec << "]\n";
                std::cout << "  div = " << std::hex << divisor << std::dec << "\n";
                std::cout << "  div_qr:  quo=[";
                for(size_t i=0; i<N; i++)
                    std::cout << std::hex << quo1[i] << (i < N-1 ? " " : "");
                std::cout << std::dec << "] rem=" << std::hex << rem1 << std::dec << "\n";
                std::cout << "  fdiv_qr: quo=[";
                for(size_t i=0; i<N; i++)
                    std::cout << std::hex << quo2[i] << (i < N-1 ? " " : "");
                std::cout << std::dec << "] rem=" << std::hex << rem2 << std::dec << "\n";
            }
            all_pass = false;
            fails++;
        }
    }
    
    if (all_pass) {
        std::cout << "PASS [" << size_name << "]: " << num_tests << " tests\n";
    } else {
        std::cout << "FAIL [" << size_name << "]: " << fails << "/" << num_tests << " failed\n";
    }
    
    return all_pass;
}

void run_all_tests() {
    std::cout << "======================\n";
    std::cout << "Correctness Tests\n";
    std::cout << "======================\n\n";
    
    std::cout << "Single-digit division tests:\n";
    bool all_pass = true;
    all_pass &= test_correctness<1>("1 word");
    all_pass &= test_correctness<2>("2 words");
    all_pass &= test_correctness<10>("10 words");
    all_pass &= test_correctness<30>("30 words");
    
    std::cout << "\nMulti-word division tests (mp_div_qr):\n";
    all_pass &= test_mp_div_qr_correctness<2, 1>("2/1 words");
    all_pass &= test_mp_div_qr_correctness<4, 2>("4/2 words");
    all_pass &= test_mp_div_qr_correctness<8, 4>("8/4 words");
    all_pass &= test_mp_div_qr_correctness<10, 5>("10/5 words");
    all_pass &= test_mp_div_qr_correctness<20, 10>("20/10 words");
    all_pass &= test_mp_div_qr_correctness<10, 2>("10/2 words");
    all_pass &= test_mp_div_qr_correctness<30, 3>("30/3 words");
    
    all_pass &= test_mp_div_qr_edge_cases();
    
    std::cout << "\n" << (all_pass ? "All tests PASSED\n" : "Some tests FAILED\n") << "\n";
}

int main() {
    run_all_tests();
    
    std::cout << "======================\n";
    std::cout << "Single-Digit Division Benchmark\n";
    std::cout << "======================\n\n";
    
    run_benchmark<1>("1 word");
    run_benchmark<2>("2 words");
    run_benchmark<10>("10 words");
    run_benchmark<30>("30 words");
    run_benchmark<100>("100 words");
    run_benchmark<200>("200 words");
    
    std::cout << "======================\n";
    std::cout << "Multi-Word Division Benchmark (mp_div_qr)\n";
    std::cout << "======================\n\n";
    
    run_mp_div_qr_benchmark<4, 2>("4/2 words");
    run_mp_div_qr_benchmark<8, 4>("8/4 words");
    run_mp_div_qr_benchmark<10, 5>("10/5 words");
    run_mp_div_qr_benchmark<20, 10>("20/10 words");
    run_mp_div_qr_benchmark<40, 20>("40/20 words");
    run_mp_div_qr_benchmark<10, 2>("10/2 words");
    run_mp_div_qr_benchmark<30, 3>("30/3 words");
    run_mp_div_qr_benchmark<100, 10>("100/10 words");
    
    return 0;
}
