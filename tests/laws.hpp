#pragma once

#include <optional>
#include <vector>

#include "prime_field.hpp"

enum class Law {
    Identity,
    Commutative,
    Associative,
    Inverse,
    Involution,
    Distributive,
    AbsorbingElement,
    DerivedOp,
    Homomorphic,
    SpecialValue,
    LeftIdentity,
    RightIdentity,
    LeftInverse,
    RightInverse,
    LeftDistributive,
    RightDistributive,
    LeftDerivedOp,
    RightDerivedOp,
};

inline const char* law_name(Law law) {
    switch (law) {
        case Law::Identity: return "identity";
        case Law::Commutative: return "commutative";
        case Law::Associative: return "associative";
        case Law::Inverse: return "inverse";
        case Law::Involution: return "involution";
        case Law::Distributive: return "distributive";
        case Law::AbsorbingElement: return "absorbing_element";
        case Law::DerivedOp: return "derived_op";
        case Law::Homomorphic: return "homomorphic";
        case Law::SpecialValue: return "special_value";
        case Law::LeftIdentity: return "left_identity";
        case Law::RightIdentity: return "right_identity";
        case Law::LeftInverse: return "left_inverse";
        case Law::RightInverse: return "right_inverse";
        case Law::LeftDistributive: return "left_distributive";
        case Law::RightDistributive: return "right_distributive";
        case Law::LeftDerivedOp: return "left_derived_op";
        case Law::RightDerivedOp: return "right_derived_op";
    }
    return "unknown";
}

enum class Operation {
    Add,
    Neg,
    Sub,
    Mul,
    Inv,
    Div,
    Legendre,
};

template<typename F>
struct LawSpec {
    Law law;

    std::optional<F> e;
    std::optional<F> a;
    std::optional<F> b;
    std::optional<F> expected;

    std::optional<Operation> base_op;
    std::optional<Operation> unary_op;

    std::optional<int> int_expected;

    explicit LawSpec(Law law) : law(law) {}

    static LawSpec identity(const F& identity_value = F::zero()) {
        LawSpec s(Law::Identity);
        s.e = identity_value;
        return s;
    }

    static LawSpec associative() { return LawSpec(Law::Associative); }
    static LawSpec commutative() { return LawSpec(Law::Commutative); }
    static LawSpec involution()  { return LawSpec(Law::Involution); }

    static LawSpec inverse_op(Operation base, const F& identity_value = F::zero()) {
        LawSpec s(Law::Inverse);
        s.base_op = base;
        s.e = identity_value;
        return s;
    }

    static LawSpec distributive(Operation add_op) {
        LawSpec s(Law::Distributive);
        s.base_op = add_op;
        return s;
    }

    static LawSpec absorbing_element(const F& zero_value = F::zero()) {
        LawSpec s(Law::AbsorbingElement);
        s.e = zero_value;
        return s;
    }

    static LawSpec derived_op(Operation base, Operation unary) {
        LawSpec s(Law::DerivedOp);
        s.base_op = base;
        s.unary_op = unary;
        return s;
    }

    static LawSpec homomorphic(Operation bin) {
        LawSpec s(Law::Homomorphic);
        s.base_op = bin;
        return s;
    }

    static LawSpec special_value(const F& va, const F& vb, const F& vexpected) {
        LawSpec s(Law::SpecialValue);
        s.a = va;
        s.b = vb;
        s.expected = vexpected;
        return s;
    }

    static LawSpec special_value(const F& va, const F& vexpected) {
        LawSpec s(Law::SpecialValue);
        s.a = va;
        s.expected = vexpected;
        return s;
    }

    static LawSpec special_value(const F& va, int vexpected) {
        LawSpec s(Law::SpecialValue);
        s.a = va;
        s.int_expected = vexpected;
        return s;
    }
};

template<typename F>
struct OperationSpec {
    Operation operation;
    std::vector<LawSpec<F>> laws;
};



// Declarations
// =============================================================================

template<typename F, typename BinOp>
bool identity(BinOp op, const F& a, const F& e);

template<typename F, typename BinOp>
bool commutative(BinOp op, const F& a, const F& b);

template<typename F, typename BinOp>
bool associative(BinOp op, const F& a, const F& b, const F& c);

template<typename F, typename BinOp, typename UnaryOp>
bool inverse(BinOp op, UnaryOp inverse_op, const F& a, const F& e);

template<typename F, typename MulOp, typename AddOp>
bool distributive(MulOp mul, AddOp add, const F& a, const F& b, const F& c);

template<typename F, typename BinOp>
bool absorbing_element(BinOp op, const F& zero, const F& a);

template<typename F, typename DerivedOp, typename BaseOp, typename UnaryOp>
bool derived_op(DerivedOp derived_op, BaseOp base_op, UnaryOp unary_op, const F& a, const F& b);

template<typename F, typename UnaryOp, typename BinOp>
bool homomorphic(UnaryOp unary_op, BinOp bin_op, const F& a, const F& b);

template<typename F, typename UnaryOp, typename BinOpDomain, typename BinOpCodomain>
bool homomorphic(UnaryOp f, BinOpDomain domain_op, BinOpCodomain codomain_op, const F& a, const F& b);

template<typename F, typename R, typename BinOp>
bool special_value(BinOp op, const F& a, const F& b, const R& expected);

template<typename F, typename R, typename UnaryOp>
bool special_value(UnaryOp op, const F& a, const R& expected);

template<typename F, typename BinOp>
bool left_identity(BinOp op, const F& a, const F& e);

template<typename F, typename BinOp>
bool right_identity(BinOp op, const F& a, const F& e);

template<typename F, typename BinOp, typename UnaryOp>
bool left_inverse(BinOp op, UnaryOp inverse_op, const F& a, const F& e);

template<typename F, typename BinOp, typename UnaryOp>
bool right_inverse(BinOp op, UnaryOp inverse_op, const F& a, const F& e);

template<typename F, typename MulOp, typename AddOp>
bool left_distributive(MulOp mul, AddOp add, const F& a, const F& b, const F& c);

template<typename F, typename MulOp, typename AddOp>
bool right_distributive(MulOp mul, AddOp add, const F& a, const F& b, const F& c);

template<typename F, typename DerivedOp, typename BaseOp, typename UnaryOp>
bool left_derived_op(DerivedOp derived_op, BaseOp base_op, UnaryOp unary_op, const F& a, const F& b);

template<typename F, typename DerivedOp, typename BaseOp, typename UnaryOp>
bool right_derived_op(DerivedOp derived_op, BaseOp base_op, UnaryOp unary_op, const F& a, const F& b);


// =============================================================================
// Implementations: Core/Standard Laws
// =============================================================================

template<typename F, typename BinOp>
bool identity(BinOp op, const F& a, const F& e) {
    return left_identity(op, a, e) && right_identity(op, a, e);
}

template<typename F, typename BinOp>
bool commutative(BinOp op, const F& a, const F& b) {
    return op(a, b) == op(b, a);
}

template<typename F, typename BinOp>
bool associative(BinOp op, const F& a, const F& b, const F& c) {
    return op(op(a, b), c) == op(a, op(b, c));
}

template<typename F, typename BinOp, typename UnaryOp>
bool inverse(BinOp op, UnaryOp inverse_op, const F& a, const F& e) {
    return left_inverse(op, inverse_op, a, e) &&
    right_inverse(op, inverse_op, a, e);
}

template<typename F, typename UnaryOp>
bool involution(UnaryOp op, const F& a) {
    return op(op(a)) == a;
}

template<typename F, typename MulOp, typename AddOp>
bool distributive(MulOp mul, AddOp add, const F& a, const F& b, const F& c) {
    return left_distributive(mul, add, a, b, c) && right_distributive(mul, add, a, b, c);
}

template<typename F, typename BinOp>
bool absorbing_element(BinOp op, const F& zero, const F& a) {
    return op(a, zero) == zero && op(zero, a) == zero;
}

template<typename F, typename DerivedOp, typename BaseOp, typename UnaryOp>
bool derived_op(DerivedOp derived_op, BaseOp base_op, UnaryOp unary_op, const F& a, const F& b) {
    return right_derived_op(derived_op, base_op, unary_op, a, b);
}

template<typename F, typename UnaryOp, typename BinOp>
bool homomorphic(UnaryOp unary_op, BinOp bin_op, const F& a, const F& b) {
    return unary_op(bin_op(a, b)) == bin_op(unary_op(a), unary_op(b));
}

template<typename F, typename UnaryOp, typename BinOpDomain, typename BinOpCodomain>
bool homomorphic(UnaryOp f, BinOpDomain domain_op, BinOpCodomain codomain_op, const F& a, const F& b) {
    return f(domain_op(a, b)) == codomain_op(f(a), f(b));
}

template<typename F, typename R, typename BinOp>
bool special_value(BinOp op, const F& a, const F& b, const R& expected) {
    return op(a, b) == expected;
}

template<typename F, typename R, typename UnaryOp>
bool special_value(UnaryOp op, const F& a, const R& expected) {
    return op(a) == expected;
}


// =============================================================================
// Implementations: Side-Aware Helpers (non-commutative)
// =============================================================================

template<typename F, typename BinOp>
bool left_identity(BinOp op, const F& a, const F& e) {
    return op(e, a) == a;
}

template<typename F, typename BinOp>
bool right_identity(BinOp op, const F& a, const F& e) {
    return op(a, e) == a;
}

template<typename F, typename BinOp, typename UnaryOp>
bool left_inverse(BinOp op, UnaryOp inverse_op, const F& a, const F& e) {
    return op(inverse_op(a), a) == e;
}

template<typename F, typename BinOp, typename UnaryOp>
bool right_inverse(BinOp op, UnaryOp inverse_op, const F& a, const F& e) {
    return op(a, inverse_op(a)) == e;
}

template<typename F, typename MulOp, typename AddOp>
bool left_distributive(MulOp mul, AddOp add, const F& a, const F& b, const F& c) {
    return mul(a, add(b, c)) == add(mul(a, b), mul(a, c));
}

template<typename F, typename MulOp, typename AddOp>
bool right_distributive(MulOp mul, AddOp add, const F& a, const F& b, const F& c) {
    return mul(add(a, b), c) == add(mul(a, c), mul(b, c));
}

template<typename F, typename DerivedOp, typename BaseOp, typename UnaryOp>
bool left_derived_op(DerivedOp derived_op, BaseOp base_op, UnaryOp unary_op, const F& a, const F& b) {
    return derived_op(a, b) == base_op(unary_op(a), b);
}

template<typename F, typename DerivedOp, typename BaseOp, typename UnaryOp>
bool right_derived_op(DerivedOp derived_op, BaseOp base_op, UnaryOp unary_op, const F& a, const F& b) {
    return derived_op(a, b) == base_op(a, unary_op(b));
}
