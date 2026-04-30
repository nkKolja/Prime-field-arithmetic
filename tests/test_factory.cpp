/**
 * Declarative operation specs using parameterized laws.
 */

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "prime_field.hpp"
#include "laws.hpp"
#include "test_common.hpp"

using namespace prime_field;

struct TestConfig {
    std::uint32_t random_sample_count = 256;
};

template<typename Prime>
std::vector<OperationSpec<FieldElement<Prime>>> operation_specs() {
    using F = FieldElement<Prime>;
    using LS = LawSpec<F>;

    return {
        { Operation::Add, {
            LS::identity(F::zero()),
            LS::associative(),
            LS::commutative(),
        }},
        { Operation::Neg, {
            LS::special_value(F::zero(), F::zero()),
            LS::involution(),
            LS::inverse_op(Operation::Add, F::zero()),
        }},
        { Operation::Sub, {
            LS::derived_op(Operation::Add, Operation::Neg),
        }},
        { Operation::Mul, {
            LS::absorbing_element(F::zero()),
            LS::identity(F::one()),
            LS::associative(),
            LS::commutative(),
            LS::distributive(Operation::Add),
        }},
        { Operation::Inv, {
            LS::special_value(F::one(), F::one()),
            LS::involution(),
            LS::inverse_op(Operation::Mul, F::one()),
        }},
        { Operation::Div, {
            LS::derived_op(Operation::Mul, Operation::Inv),
        }},
        { Operation::Legendre, {
            LS::special_value(F::one(), (int)1),
            LS::homomorphic(Operation::Mul),
        }},
    };
}

template<typename Prime>
const char* operation_name(Operation op) {
    (void)sizeof(Prime);
    switch (op) {
        case Operation::Add: return "add";
        case Operation::Neg: return "neg";
        case Operation::Sub: return "sub";
        case Operation::Mul: return "mul";
        case Operation::Inv: return "inv";
        case Operation::Div: return "div";
        case Operation::Legendre: return "legendre";
    }
    return "unknown";
}

template<typename Prime>
auto binary_op_for(Operation op) {
    using F = FieldElement<Prime>;
    return [op](const F& x, const F& y) {
        F out;
        switch (op) {
            case Operation::Add: add(out, x, y); return out;
            case Operation::Sub: sub(out, x, y); return out;
            case Operation::Mul: mul(out, x, y); return out;
            case Operation::Div: div(out, x, y); return out;
            default: return x;
        }
    };
}

template<typename Prime>
auto unary_op_for(Operation op) {
    using F = FieldElement<Prime>;
    return [op](const F& x) {
        F out;
        switch (op) {
            case Operation::Neg: neg(out, x); return out;
            case Operation::Inv: inv(out, x); return out;
            default: return x;
        }
    };
}

template<typename Prime>
bool evaluate_law(Operation op,
                  const LawSpec<FieldElement<Prime>>& spec,
                  const FieldElement<Prime>& a,
                  const FieldElement<Prime>& b,
                  const FieldElement<Prime>& c) {

    const auto bin = binary_op_for<Prime>(op);
    const auto un  = unary_op_for<Prime>(op);

    switch (spec.law) {
        case Law::Identity:
            return spec.e.has_value() && identity(bin, a, *spec.e);
        case Law::Commutative:
            return commutative(bin, a, b);
        case Law::Associative:
            return associative(bin, a, b, c);
        case Law::Involution:
            return involution(un, a);
        case Law::Inverse:
            return spec.base_op.has_value() && spec.e.has_value() && inverse(binary_op_for<Prime>(*spec.base_op), un, a, *spec.e);
        case Law::Distributive:
            return spec.base_op.has_value() && distributive(bin, binary_op_for<Prime>(*spec.base_op), a, b, c);
        case Law::AbsorbingElement:
            return spec.e.has_value() && absorbing_element(bin, *spec.e, a);
        case Law::DerivedOp:
            return spec.base_op.has_value() && spec.unary_op.has_value() &&
                   derived_op(bin, binary_op_for<Prime>(*spec.base_op), unary_op_for<Prime>(*spec.unary_op), a, b);
        case Law::Homomorphic:
            if (!spec.base_op.has_value()) return false;
            if (op == Operation::Legendre) {
                auto leg = [](const auto& x) { return legendre(x); };
                return homomorphic(leg, binary_op_for<Prime>(*spec.base_op), [](int x, int y) { return x * y; }, a, b);
            }
            return homomorphic(un, binary_op_for<Prime>(*spec.base_op), a, b);
        case Law::SpecialValue:
            if (!spec.a.has_value()) return false;
            if (spec.int_expected.has_value()) {
                auto leg = [](const auto& x) { return legendre(x); };
                return special_value(leg, *spec.a, *spec.int_expected);
            }
            if (!spec.expected.has_value()) return false;
            if (spec.b.has_value()) {
                return special_value(bin, *spec.a, *spec.b, *spec.expected);
            }
            return special_value(un, *spec.a, *spec.expected);
        default:
            return false;
    }
}

template<typename Prime>
struct LawRunResult {
    bool ok = true;
    std::uint32_t sample_index = 0;
    FieldElement<Prime> a{};
    FieldElement<Prime> b{};
    FieldElement<Prime> c{};
};

template<typename Prime>
LawRunResult<Prime> run_law(Operation op,
                            const LawSpec<FieldElement<Prime>>& spec,
                            std::uint32_t random_sample_count) {
    using F = FieldElement<Prime>;
    LawRunResult<Prime> result{};

    if (spec.law == Law::SpecialValue) {
        result.a = spec.a.value_or(F{});
        result.b = spec.b.value_or(F{});
        result.c = F{};
        result.ok = evaluate_law<Prime>(op, spec, result.a, result.b, result.c);
        return result;
    }

    for (std::uint32_t i = 0; i < random_sample_count; ++i) {
        result.sample_index = i;
        result.a = F::random();
        result.b = F::random();
        result.c = F::random();
        result.ok = evaluate_law<Prime>(op, spec, result.a, result.b, result.c);
        if (!result.ok) {
            return result;
        }
    }
    return result;
}

template<typename Prime>
void print_failure_details(Operation op,
                           const LawSpec<FieldElement<Prime>>& spec,
                           const LawRunResult<Prime>& result) {
    std::cerr << "\nFAILED LAW\n";
    std::cerr << "operation: " << operation_name<Prime>(op) << "\n";
    std::cerr << "law: " << law_name(spec.law) << "\n";
    std::cerr << "sample_index: " << result.sample_index << "\n";
    std::cerr << "a: " << result.a << "\n";
    std::cerr << "b: " << result.b << "\n";
    std::cerr << "c: " << result.c << "\n";

    if (spec.e.has_value()) {
        std::cerr << "e: " << *spec.e << "\n";
    }
    if (spec.expected.has_value()) {
        std::cerr << "expected: " << *spec.expected << "\n";
    }
    if (spec.int_expected.has_value()) {
        std::cerr << "expected_int: " << *spec.int_expected << "\n";
    }
    if (spec.base_op.has_value()) {
        std::cerr << "base_op: " << operation_name<Prime>(*spec.base_op) << "\n";
    }
    if (spec.unary_op.has_value()) {
        std::cerr << "unary_op: " << operation_name<Prime>(*spec.unary_op) << "\n";
    }
}

template<typename Prime>
int run_specs(const TestConfig& config) {
    g_test_state = TestState{};

    const auto specs = operation_specs<Prime>();
    const int num_functions = static_cast<int>(specs.size());

    std::vector<std::string> function_name_storage;
    function_name_storage.reserve(specs.size());

    std::vector<const char*> function_names;
    function_names.reserve(specs.size());

    std::vector<int> tests_per_function;
    tests_per_function.reserve(specs.size());

    int total_tests = 0;
    int fn_idx = 0;
    
    for (const auto& spec : specs) {
        function_name_storage.push_back(operation_name<Prime>(spec.operation));
        function_names.push_back(function_name_storage.back().c_str());
        tests_per_function.push_back(static_cast<int>(spec.laws.size()));
        total_tests += static_cast<int>(spec.laws.size());

        int law_idx = 0;
        for (const auto& law_spec : spec.laws) {
            const auto result = run_law<Prime>(spec.operation, law_spec, config.random_sample_count);
            const bool ok = result.ok;

            const std::string test_name =
                function_name_storage.back() + "::" + law_name(law_spec.law);
            TEST(fn_idx, law_idx, test_name.c_str(), ok);
            if (!ok) {
                print_failure_details<Prime>(spec.operation, law_spec, result);
                return 1;
            }
            law_idx++;
        }
        fn_idx++;
    }

    print_test_summary(
        "factory laws",
        total_tests,
        num_functions,
        function_names.data(),
        tests_per_function.data());

    return g_test_state.failed_tests.empty() ? 0 : 1;
}

int main() {
#ifdef PRIME_TYPE
    const TestConfig config{};

    return run_specs<PRIME_TYPE>(config);
#endif
    return 0;
}
