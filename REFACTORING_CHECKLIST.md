# Prime-field-arithmetic — Refactoring & Release Checklist

Purpose: turn the current research/refactor branch into a small, production-like C++ header/library. Prioritize minimal runnable examples, modern CMake packaging, tests, CI, and clear documentation.

Legend
- [ ]: not started  - [~]: in progress  - [x]: done

Top priorities (interview-ready, do these first)
- [ ] Examples: add two minimal, runnable examples (build + run in < 2 minutes):
  - example/basic_usage/example_arithmetic.cpp — add/sub/mul/div, basic API usage
  - example/basic_usage/example_field_ops.cpp — inverse, pow, sqrt (if available)
- [ ] Modern CMake + Packaging:
  - `CMakeLists.txt`: require CMake >= 3.15, add `prime_field::prime_field` INTERFACE target
  - `install()` headers and export targets
  - add `cmake/prime_field-config.cmake.in` and `prime_field-config-version.cmake.in`
- [ ] CI: GitHub Actions that build (Debug/Release), run unit tests, and build/run examples
- [ ] README: short Quick Start showing how to build, run examples, and use `find_package(PrimeField)` after install

API & code hygiene (short-term)
- [ ] Public API review: finalise `include/prime_field.hpp` surface — small, clear, documented entry point
- [ ] Split operators into `include/prime_field/operators/` (arithmetic, comparison, io)
- [ ] Add `include/prime_field/traits/` with basic type traits and compile-time checks
- [ ] Move internals to `include/prime_field/detail/`; add `config.hpp` and `macros.hpp`
- [ ] Ensure all headers have include guards or `#pragma once`
- [ ] Add lightweight Doxygen-style comments to public classes/functions for quick reference

Testing & Validation
- [ ] Unit tests: expand `tests/` to cover basic algebraic properties (associativity, distributivity, inverses)
- [ ] Add small deterministic RNG seeds for reproducible tests
- [ ] Benchmarks: include microbenchmarks for add/mul/inv on typical primes (benchmarks/)
- [ ] Automate benchmark run in CI as optional workflow (release-only)

Performance & correctness (medium-term)
- [ ] Document and verify any RM or Montgomery optimisation implementations with correctness proofs/tests
- [ ] Add performance regression tests to detect accidental slowdowns
- [ ] Provide benchmark results in `docs/guides/PERFORMANCE.md`

Documentation & Examples (medium-term)
- [ ] Docs skeleton: `docs/` with `guides/`, `api/`, `overview/`
- [ ] `docs/guides/GETTING_STARTED.md` — quick start, install, example run
- [ ] `docs/api/FIELD_ELEMENT.md` — describe `FieldElement` API and semantics
- [ ] Add simple usage snippets inside headers as short examples

Build system & packaging (medium-term)
- [ ] Add `package()` / `CPack` recipe if you want binary packaging for demos
- [ ] Provide minimal `Makefile` or `bootstrap.sh` for local dev convenience

Code quality & developer experience
- [ ] Add `.clang-format` and apply it
- [ ] Add `.clang-tidy` recommended checks; fix high-confidence issues
- [ ] Add CONTRIBUTING.md with build/test/benchmark instructions
- [ ] Add license headers to source files and verify `LICENSE`

CI/CD & Release
- [ ] CI: build matrix (linux/macOS, Debug/Release), run unit tests and examples
- [ ] Add status badges to `README.md`
- [ ] Automate release tagging on merge (optionally via a GitHub Action)

Acceptance criteria (what 'done' looks like for interview)
- [ ] `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --target install` installs headers and exports `prime_field::prime_field`
- [ ] Examples compile and run against installed package in < 2 minutes on a laptop
- [ ] Unit tests pass in CI on Linux and macOS
- [ ] Brief README with 3 commands: build, install, run example
- [ ] One-page demo notes that include: API decisions, where optimizations live, and a short performance summary

Demo checklist (prep for interview)
- [ ] One-liner elevator pitch about design choices (namespacing, header-only vs binary, per-prime implementations)
- [ ] Have commands ready to build & run example (copy/paste):
  - mkdir -p build && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  - cmake --build build --target install
  - cmake --build build --target example_basic_usage

Optional / Nice-to-have (if time permits)
- [ ] Provide `conan` or `vcpkg` manifest for consumers
- [ ] Add continuous benchmark reporting (e.g., use google/benchmark + upload results)
- [ ] Add code coverage job and badge

Notes & priorities
- Focus immediate effort on Examples + CMake + CI + README — these maximize interview impact.
- After those, prioritize API ergonomics, tests, and docs.

