.PHONY: all clean test bench test_all bench_all

CXX = g++
CXXFLAGS = -std=c++20 -I./include -O3 -Wfatal-errors

# Build directory
BUILD_DIR = build

# All primes
PRIMES = p64_0 p64_1 p128_0 p128_1 p192_0 p192_1 p256_0 p256_1 p512_0 p512_1

# Test and benchmark binaries in build directory
TEST_BINS = $(addprefix $(BUILD_DIR)/test_,$(PRIMES))
BENCH_BINS = $(addprefix $(BUILD_DIR)/bench_,$(PRIMES))

# Dependencies
DEPS = include/prime_field/field_element.hpp \
       include/prime_field/field_element.tpp \
       include/prime_field/params.hpp \
       include/prime_field/types.hpp \
       include/prime_field/detail/helpers.hpp \
       $(wildcard include/prime_field/primes/*.hpp)

all: test_all

# Ensure build directory exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Pattern rule for test binaries
$(BUILD_DIR)/test_%: tests/tests.cpp $(DEPS) | $(BUILD_DIR)
	@echo "Building test for $*..."
	@$(CXX) $(CXXFLAGS) -DPRIME_TYPE=$(shell echo $* | tr 'a-z' 'A-Z') -o $@ $<

# Pattern rule for benchmark binaries
$(BUILD_DIR)/bench_%: benchmarks/bench.cpp $(DEPS) | $(BUILD_DIR)
	@echo "Building benchmark for $*..."
	@$(CXX) $(CXXFLAGS) -DPRIME_TYPE=$(shell echo $* | tr 'a-z' 'A-Z') -o $@ $<

# Convenience targets without build/ prefix
define make_test_target
test_$(1): $(BUILD_DIR)/test_$(1)
endef

define make_bench_target
bench_$(1): $(BUILD_DIR)/bench_$(1)
endef

$(foreach prime,$(PRIMES),$(eval $(call make_test_target,$(prime))))
$(foreach prime,$(PRIMES),$(eval $(call make_bench_target,$(prime))))

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
	@echo "  make            - Build all tests"
	@echo "  make test       - Run all tests"
	@echo "  make bench      - Run all benchmarks"
	@echo "  make test_<prime>  - Build and run test for specific prime (e.g., test_p64_0)"
	@echo "  make bench_<prime> - Build and run benchmark for specific prime (e.g., bench_p64_0)"
	@echo "  make clean      - Remove build directory"
