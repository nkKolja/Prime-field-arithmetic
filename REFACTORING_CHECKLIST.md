# Repository Refactoring Checklist

Refactoring Prime-field-arithmetic to match professional cupqc SDK structure for interview demonstration.

## Phase 1: Directory Structure Reorganization

### Include Directory Namespace
- [x] Create `include/prime_field/` namespace directory
- [x] Move `field_element.hpp` to `include/prime_field/`
- [x] Move `field_element.tpp` to `include/prime_field/`
- [x] Move `params.hpp` to `include/prime_field/`
- [x] Move `types.hpp` to `include/prime_field/`
- [x] Move `helpers.hpp` to `include/prime_field/detail/`
- [x] Create `include/prime_field/operators/` directory
- [x] Create `include/prime_field/traits/` directory
- [x] Create `include/prime_field/detail/` directory
- [x] Move `primes/` directory to `include/prime_field/primes/`

### Main Header
- [x] Create `include/prime_field.hpp` as main entry point
- [x] Add header guards and includes for all public headers
- [x] Add namespace documentation

### Examples Directory
- [ ] Create `example/` directory
- [ ] Create `example/basic_usage/` subdirectory
- [ ] Create `example/basic_usage/example_arithmetic.cpp` (add, sub, mul, div)
- [ ] Create `example/basic_usage/example_montgomery.cpp` (conversions)
- [ ] Create `example/basic_usage/example_field_ops.cpp` (inv, pow, sqrt)
- [ ] Create `example/basic_usage/Makefile`
- [ ] Create `example/advanced/` subdirectory
- [ ] Create `example/advanced/example_custom_prime.cpp`
- [ ] Create `example/advanced/example_performance.cpp`
- [ ] Create `example/advanced/Makefile`

### CMake Infrastructure
- [ ] Create `cmake/` directory
- [ ] Create `cmake/prime_field-config.cmake.in` (package config template)
- [ ] Create `cmake/prime_field-config-version.cmake.in` (version config)
- [ ] Create `cmake/prime_field-targets.cmake` (export targets)

### Documentation Structure
- [ ] Create `docs/` directory
- [ ] Create `docs/api/` subdirectory
- [ ] Create `docs/guides/` subdirectory
- [ ] Create `docs/overview/` subdirectory

## Phase 2: Code Organization

### Traits System
- [ ] Create `include/prime_field/traits/field_traits.hpp`
- [ ] Add type traits for field element properties
- [ ] Add concepts for C++20 (if using) or SFINAE helpers
- [ ] Create `include/prime_field/traits/prime_traits.hpp`
- [ ] Add compile-time prime validation traits

### Operator Organization
- [ ] Create `include/prime_field/operators/arithmetic.hpp`
- [ ] Move arithmetic operators (+, -, *, /) to separate file
- [ ] Create `include/prime_field/operators/comparison.hpp`
- [ ] Move comparison operators (==, !=, <, >, <=, >=) to separate file
- [ ] Create `include/prime_field/operators/io.hpp`
- [ ] Add stream operators (<<, >>) for field elements

### Detail Namespace
- [ ] Move internal helpers to `include/prime_field/detail/`
- [ ] Create `include/prime_field/detail/config.hpp` for build configuration
- [ ] Create `include/prime_field/detail/macros.hpp` for internal macros
- [ ] Update helper functions to use detail namespace

### Header Dependencies
- [ ] Update all `#include` paths in moved files
- [ ] Ensure forward declarations where appropriate
- [ ] Add include guards to all headers
- [ ] Add `#pragma once` or traditional guards consistently

## Phase 3: Professional Polish

### CMake Build System
- [ ] Update `CMakeLists.txt` with modern CMake practices (3.15+)
- [ ] Add `install()` targets for headers
- [ ] Add `export()` for package configuration
- [ ] Configure package config file generation
- [ ] Add version configuration
- [ ] Create `prime_field::prime_field` INTERFACE library target
- [ ] Add installation rules for cmake config files
- [ ] Add `find_package(PrimeField)` support

### Documentation Files
- [ ] Update `README.md` with professional structure:
  - [ ] Project overview and features
  - [ ] Quick start example
  - [ ] Installation instructions
  - [ ] Usage examples with code snippets
  - [ ] API reference links
  - [ ] Building from source
  - [ ] Running tests and benchmarks
  - [ ] Contributing guidelines
  - [ ] License information
- [ ] Create `docs/api/FIELD_ELEMENT.md` (FieldElement API reference)
- [ ] Create `docs/api/OPERATORS.md` (operator overloads)
- [ ] Create `docs/api/PRIMES.md` (available prime configurations)
- [ ] Create `docs/guides/GETTING_STARTED.md`
- [ ] Create `docs/guides/ADDING_PRIMES.md`
- [ ] Create `docs/guides/PERFORMANCE.md`
- [ ] Create `docs/overview/FEATURES.md`
- [ ] Create `docs/overview/REQUIREMENTS.md`

### License and Legal
- [ ] Review/update `LICENSE` file
- [ ] Add license headers to all source files
- [ ] Create `NOTICE` file if using third-party code
- [ ] Add copyright notices

### Examples
- [ ] Write comprehensive example code
- [ ] Ensure all examples compile and run
- [ ] Add comments and documentation to examples
- [ ] Create example Makefiles that use installed library

### CI/CD Improvements
- [ ] Update `.github/workflows/ci.yml`:
  - [ ] Add installation test (make install)
  - [ ] Add example building test
  - [ ] Add documentation generation (if using doxygen/sphinx)
  - [ ] Add code coverage reporting (optional)
- [ ] Add status badges to README (build status, license)

### Code Quality
- [ ] Add `.clang-format` for consistent formatting
- [ ] Run formatter on all code
- [ ] Add `.clang-tidy` configuration
- [ ] Address any warnings or suggestions
- [ ] Add `CHANGELOG.md` for version history

### Build System Updates
- [ ] Update Makefile to respect new structure
- [ ] Add `make install` target to Makefile
- [ ] Add `make examples` target
- [ ] Ensure tests work with new structure
- [ ] Ensure benchmarks work with new structure

## Phase 4: Validation

### Build Testing
- [ ] Clean build with CMake
- [ ] Clean build with Makefile
- [ ] Test all 10 prime configurations
- [ ] Run all tests and verify passing
- [ ] Run all benchmarks
- [ ] Build all examples

### Installation Testing
- [ ] Test `make install` / `cmake --install`
- [ ] Create test project using `find_package(PrimeField)`
- [ ] Verify installed headers are accessible
- [ ] Verify examples work with installed library

### Documentation Review
- [ ] Proofread all documentation
- [ ] Verify all code examples compile
- [ ] Check all links work
- [ ] Ensure consistent terminology

### Final Checks
- [ ] All files have appropriate licenses
- [ ] No build warnings
- [ ] No TODOs or FIXMEs in committed code
- [ ] Git history is clean
- [ ] Tag release version (e.g., v1.0.0)

---

## Notes

- **Priority**: Focus on Phase 1-2 first for maximum impact
- **Timeline**: Estimate 2-3 hours for full refactoring
- **Testing**: Run tests after each major change
- **Git**: Commit frequently with clear messages
- **Interview**: Highlight namespace organization, modern CMake, and professional structure
