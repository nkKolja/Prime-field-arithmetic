#pragma once

#include <array>
#include <iostream>
#include <iomanip>
#include "types.hpp"
#include "helpers.hpp"

namespace prime_field {

// Forward declarations
template<typename Prime> class FieldElement;
template<typename Prime> void reduce(FieldElement<Prime>& a);
template<typename Prime> void reduce_full(FieldElement<Prime>& a);
template<typename Prime> void add(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2);
template<typename Prime> void sub(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2);
template<typename Prime> void mul(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2);
template<typename Prime> void div(FieldElement<Prime>& out, const FieldElement<Prime>& in1, const FieldElement<Prime>& in2);
template<typename Prime> void neg(FieldElement<Prime>& out, const FieldElement<Prime>& in);
template<typename Prime> void inv(FieldElement<Prime>& out, const FieldElement<Prime>& in);
template<typename Prime> void pow(FieldElement<Prime>& out, const FieldElement<Prime>& in, const std::array<digit_t, Prime::NWORDS>& exp);
template<typename Prime> void sqrt(FieldElement<Prime>& out, const FieldElement<Prime>& in);
template<typename Prime> int legendre(const FieldElement<Prime>& a);
template<typename Prime> FieldElement<Prime> random();
template<typename Prime> void conditional_select(FieldElement<Prime>& out, const FieldElement<Prime>& in1, FieldElement<Prime>& in2, bool cond);
template<typename Prime> FieldElement<Prime> to_montgomery(const std::array<digit_t, Prime::NWORDS>& value);
template<typename Prime> std::array<digit_t, Prime::NWORDS> from_montgomery(const FieldElement<Prime>& a);

/**
 * Type-safe field element for prime field arithmetic.
 * Template parameter Prime determines the field.
 */
template<typename Prime>
class FieldElement {
public:
    static constexpr size_t NWORDS = Prime::NWORDS;
    static constexpr size_t NBITS = Prime::NBITS;
    std::array<digit_t, NWORDS> data;
    
    // Constructors
    constexpr FieldElement() : data{} {}
    
    explicit FieldElement(const std::array<digit_t, NWORDS>& arr) : data(arr) {
        *this = to_montgomery<Prime>(arr);
    }
    
    explicit FieldElement(digit_t value) {
        std::array<digit_t, NWORDS> arr{};
        arr[0] = value;
        *this = to_montgomery<Prime>(arr);
    }

    constexpr FieldElement(const FieldElement& other) = default;
    
    // Arithmetic operators
    FieldElement operator+(const FieldElement& other) const {
        FieldElement result;
        add(result, *this, other);
        return result;
    }
    FieldElement& operator+=(const FieldElement& other) {
        FieldElement result;
        add(result, *this, other);
        return *this = result;;
    }

    FieldElement operator-() const {
        FieldElement result;
        neg(result, *this);
        return result;
    }

    FieldElement operator-(const FieldElement& other) const {
        FieldElement result;
        sub(result, *this, other);
        return result;
    }
    FieldElement& operator-=(const FieldElement& other) {
        FieldElement result;
        sub(result, *this, other);
        return *this = result;;
    }

    FieldElement operator*(const FieldElement& other) const {
        FieldElement result;
        mul(result,*this, other);
        return result;
    }
    FieldElement& operator*=(const FieldElement& other) {
        FieldElement result;
        mul(result, *this, other);
        return *this = result;
    }

    
    FieldElement operator/(const FieldElement& other) const {
        FieldElement inv_other = other.inverse();
        return *this * inv_other;
    }
    FieldElement& operator/=(const FieldElement& other) {
        *this = *this / other;
        return *this;
    }
    
    
    // Comparison
    bool operator==(const FieldElement& other) const {
        volatile digit_t c = 0;
        for (size_t i = 0; i < NWORDS; i++)
            c |= data[i] ^ other.data[i];
        return ((c | (0 - c)) >> (RADIX - 1)) ^ 1;
    }
    
    bool operator!=(const FieldElement& other) const {
        volatile digit_t c = 0;
        for (size_t i = 0; i < NWORDS; i++)
            c |= data[i] ^ other.data[i];
        return ((c | (0 - c)) >> (RADIX - 1)) ^ 0;
    }
    
    // Test if non-zero
    explicit operator bool() const {
        volatile digit_t c = 0;
        for (size_t i = 0; i < NWORDS; i++)
            c |= data[i];
        return ((c | (0 - c)) >> (RADIX - 1)) ^ 0;
    }
    
    // Additional operations
    FieldElement inverse() const {
        FieldElement result;
        inv(result, *this);
        return result;
    }
    
    digit_t legendre() const {
        return prime_field::legendre(*this);
    }

    FieldElement sqrt() const {
        FieldElement result;
        prime_field::sqrt(result, *this);
        return result;
    }
    
    
    void print(std::ostream& os = std::cout) const {
        print_array(os, data);
    }
};

// Stream output operator
template<typename Prime>
std::ostream& operator<<(std::ostream& os, const FieldElement<Prime>& elem) {
    elem.print(os);
    return os;
}

} // namespace prime_field

// Include template implementations
#include "field_element.tpp"
