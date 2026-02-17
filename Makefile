.PHONY: all clean test bench test_all bench_all example test_primitives test_mp bench_div

CXX = g++
CXXFLAGS = -std=c++20 -I./include -O3 -Wfatal-errors

# Build directory
BUILD_DIR = build

# All primes
PRIMES = p64_0 p64_1 p128_0 p128_1 p192_0 p192_1 p256_0 p256_1 p512_0 p512_1

# Test and benchmark binaries in build directory
TEST_BINS = $(addprefix $(BUILD_DIR)/test_,$(PRIMES))
BENCH_BINS = $(addprefix $(BUILD_DIR)/bench_,$(PRIMES))
PRIMITIVES_TEST = $(BUILD_DIR)/test_primitives
MP_TEST = $(BUILD_DIR)/test_mp
DIV_BENCH = $(BUILD_DIR)/bench_div

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

# Build primitives test
$(PRIMITIVES_TEST): tests/test_primitives.cpp include/common/primitives.hpp include/common/types.hpp include/common/config.hpp | $(BUILD_DIR)
	@echo "Building primitives test..."
	@$(CXX) $(CXXFLAGS) -o $@ tests/test_primitives.cpp

# Primitives test convenience target
test_primitives: $(PRIMITIVES_TEST)
	@echo ""
	@echo "Running primitives test..."
	@$(PRIMITIVES_TEST)

# Build mp library test
$(MP_TEST): tests/test_mp.cpp include/mp/mp.hpp include/mp/*.hpp include/common/primitives.hpp include/common/types.hpp include/common/config.hpp | $(BUILD_DIR)
	@echo "Building mp library test..."
	@$(CXX) $(CXXFLAGS) -o $@ tests/test_mp.cpp

# MP library test convenience target
test_mp: $(MP_TEST)
	@echo ""
	@echo "Running mp library test..."
	@$(MP_TEST)

# Build division benchmark
$(DIV_BENCH): benchmarks/bench_div.cpp include/mp/arithmetic.hpp include/common/primitives.hpp include/common/types.hpp include/common/config.hpp | $(BUILD_DIR)
	@echo "Building division benchmark..."
	@$(CXX) $(CXXFLAGS) -o $@ benchmarks/bench_div.cpp

# Division benchmark convenience target
bench_div: $(DIV_BENCH)
	@echo ""
	@echo "Running division benchmark..."
	@$(DIV_BENCH)

# Build and run all tests
test_all: $(TEST_BINS) test_primitives test_mp
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

# Build and run the basic example
example:
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(BUILD_DIR) --target example_basic_usage -- -j
	@./$(BUILD_DIR)/example/basic_usage/example_basic_usage

help:
	@echo "Available targets:"
	@echo "  make            - Build all tests"
	@echo "  make test       - Run all tests"
	@echo "  make test_primitives - Run primitives test"
	@echo "  make test_mp    - Run mp library test"
	@echo "  make bench      - Run all benchmarks"
	@echo "  make bench_div  - Run division benchmark"
	@echo "  make test_<prime>  - Build and run test for specific prime (e.g., test_p64_0)"
	@echo "  make bench_<prime> - Build and run benchmark for specific prime (e.g., bench_p64_0)"
	@echo "  make clean      - Remove build directory"
