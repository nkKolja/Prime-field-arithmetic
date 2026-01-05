.PHONY: all clean test bench test_all bench_all

CXX = g++
CXXFLAGS = -std=c++20 -I./include -O3 -Wfatal-errors

# Build directory
BUILD_DIR = build

# Prime IDs
P64_0_ID = 0
P64_1_ID = 1
P128_0_ID = 2
P128_1_ID = 3
P192_0_ID = 4
P192_1_ID = 5
P256_0_ID = 6
P256_1_ID = 7
P512_0_ID = 8
P512_1_ID = 9

# All primes
PRIMES = p64_0 p64_1 p128_0 p128_1 p192_0 p192_1 p256_0 p256_1 p512_0 p512_1

# Test and benchmark binaries in build directory
TEST_BINS = $(addprefix $(BUILD_DIR)/test_,$(PRIMES))
BENCH_BINS = $(addprefix $(BUILD_DIR)/bench_,$(PRIMES))

# Dependencies
DEPS = include/field_element.hpp include/field_element.tpp include/primes.hpp include/helpers.hpp include/types.hpp

all: test_all

# Ensure build directory exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Individual test targets
$(BUILD_DIR)/test_p64_0: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P64_0_ID) -o $@ $<

$(BUILD_DIR)/test_p64_1: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P64_1_ID) -o $@ $<

$(BUILD_DIR)/test_p128_0: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P128_0_ID) -o $@ $<

$(BUILD_DIR)/test_p128_1: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P128_1_ID) -o $@ $<

$(BUILD_DIR)/test_p192_0: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P192_0_ID) -o $@ $<

$(BUILD_DIR)/test_p192_1: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P192_1_ID) -o $@ $<

$(BUILD_DIR)/test_p256_0: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P256_0_ID) -o $@ $<

$(BUILD_DIR)/test_p256_1: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P256_1_ID) -o $@ $<

$(BUILD_DIR)/test_p512_0: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P512_0_ID) -o $@ $<

$(BUILD_DIR)/test_p512_1: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P512_1_ID) -o $@ $<

# Individual benchmark targets
$(BUILD_DIR)/bench_p64_0: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P64_0_ID) -o $@ $<

$(BUILD_DIR)/bench_p64_1: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P64_1_ID) -o $@ $<

$(BUILD_DIR)/bench_p128_0: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P128_0_ID) -o $@ $<

$(BUILD_DIR)/bench_p128_1: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P128_1_ID) -o $@ $<

$(BUILD_DIR)/bench_p192_0: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P192_0_ID) -o $@ $<

$(BUILD_DIR)/bench_p192_1: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P192_1_ID) -o $@ $<

$(BUILD_DIR)/bench_p256_0: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P256_0_ID) -o $@ $<

$(BUILD_DIR)/bench_p256_1: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P256_1_ID) -o $@ $<

$(BUILD_DIR)/bench_p512_0: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P512_0_ID) -o $@ $<

$(BUILD_DIR)/bench_p512_1: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -DPRIME_ID=$(P512_1_ID) -o $@ $<

# Convenience targets without build/ prefix
test_p64_0: $(BUILD_DIR)/test_p64_0
test_p64_1: $(BUILD_DIR)/test_p64_1
test_p128_0: $(BUILD_DIR)/test_p128_0
test_p128_1: $(BUILD_DIR)/test_p128_1
test_p192_0: $(BUILD_DIR)/test_p192_0
test_p192_1: $(BUILD_DIR)/test_p192_1
test_p256_0: $(BUILD_DIR)/test_p256_0
test_p256_1: $(BUILD_DIR)/test_p256_1
test_p512_0: $(BUILD_DIR)/test_p512_0
test_p512_1: $(BUILD_DIR)/test_p512_1

bench_p64_0: $(BUILD_DIR)/bench_p64_0
bench_p64_1: $(BUILD_DIR)/bench_p64_1
bench_p128_0: $(BUILD_DIR)/bench_p128_0
bench_p128_1: $(BUILD_DIR)/bench_p128_1
bench_p192_0: $(BUILD_DIR)/bench_p192_0
bench_p192_1: $(BUILD_DIR)/bench_p192_1
bench_p256_0: $(BUILD_DIR)/bench_p256_0
bench_p256_1: $(BUILD_DIR)/bench_p256_1
bench_p512_0: $(BUILD_DIR)/bench_p512_0
bench_p512_1: $(BUILD_DIR)/bench_p512_1

# Build and run all tests
test_all: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo ""; \
		echo "Running $$test..."; \
		$$test || exit 1; \
	done

# Build and run all benchmarks
bench_all: $(BENCH_BINS)
	@for bench in $(BENCH_BINS); do \
		echo ""; \
		echo "Running $$bench..."; \
		$$bench; \
	done

# Main test target runs all tests
test: test_all

# Main bench target runs all benchmarks
bench: bench_all

clean:
	@rm -rf $(BUILD_DIR)

help:
	@echo "Available targets:"
	@echo "  make       - Build tests"
	@echo "  make test  - Run tests"
	@echo "  make bench - Run benchmarks"
	@echo "  make clean - Remove binaries"
