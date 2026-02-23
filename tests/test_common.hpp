#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <string>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_GRAY    "\033[90m"

// Pass/fail/skip markers
#define PASS(skipped, failed) ((skipped) ? COLOR_GRAY "-" COLOR_RESET : ((failed) ? COLOR_RED "✗" COLOR_RESET : COLOR_GREEN "✔" COLOR_RESET))

// Test state structure
struct TestState {
    static constexpr int MAX_FUNCTIONS = 64;
    static constexpr int MAX_TESTS_PER_FUNCTION = 256;
    static constexpr int TESTS_PAD = 19;

    bool tests[MAX_FUNCTIONS][MAX_TESTS_PER_FUNCTION];
    bool function_failed[MAX_FUNCTIONS];
    bool function_skipped[MAX_FUNCTIONS];

    struct FailureInfo {
        std::string name;
        int line;
    };
    std::vector<FailureInfo> failed_tests;
    
    TestState() {
        memset(tests, 0, sizeof(tests));
        memset(function_failed, 0, sizeof(function_failed));
        memset(function_skipped, 0, sizeof(function_skipped));
    }
    
    const char* pass_check(int fn_idx, int n) {
        static char buffer[256] = "";
        buffer[0] = '\0';
        for(int i = 0; i < TESTS_PAD - n; i++)
            strcat(buffer, " ");
        for(int i = 0; i < n; i++)
            strcat(buffer, PASS(function_skipped[fn_idx], tests[fn_idx][i]));
        return buffer;
    }
};

// Global test state
static TestState g_test_state;

// Test macro
#define TEST(fn_idx, test_idx, name, condition) \
    do { \
        if (g_test_state.function_skipped[fn_idx]) break; \
        if (!(condition)) { \
            g_test_state.tests[fn_idx][test_idx] = true; \
            g_test_state.function_failed[fn_idx] = true; \
            g_test_state.failed_tests.push_back({name, __LINE__}); \
        } \
    } while(0)

// Skip if dependencies failed
#define SKIP_IF_FAILED(...) \
    do { \
        int deps[] = {__VA_ARGS__}; \
        for (size_t i = 0; i < sizeof(deps)/sizeof(deps[0]); i++) { \
            if (g_test_state.function_failed[deps[i]]) { \
                g_test_state.function_skipped[fn_idx] = true; \
                return; \
            } \
        } \
    } while(0)

// Print test summary
inline void print_test_summary(const char* test_suite_name, int total_tests, int num_functions, const char** function_names, const int* tests_per_function) {
    std::cout << "\n=====================\n";
    std::cout << "Testing " << test_suite_name << "\n";
    std::cout << "RADIX = " << 64 << " bits\n";
    std::cout << "=====================\n\n";
    
    for (int i = 0; i < num_functions; i++) {
        std::cout << function_names[i] << g_test_state.pass_check(i, tests_per_function[i]) << "\n";
    }
    
    std::cout << "\n";
    
    if (g_test_state.failed_tests.empty()) {
        std::cout << COLOR_GREEN "✔ All " << total_tests << " tests passed!" COLOR_RESET "\n\n";
    } else {
        std::cout << COLOR_RED "✗ " << g_test_state.failed_tests.size() << " test(s) failed:" COLOR_RESET "\n";
        for (const auto& failure : g_test_state.failed_tests) {
            std::cout << "  Line " << failure.line << ": " << failure.name << "\n";
        }
        std::cout << "\n";
    }
}
