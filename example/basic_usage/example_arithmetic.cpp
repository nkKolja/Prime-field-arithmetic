#include <iostream>
#include "prime_field/field_element.hpp"
#include "prime_field/detail/helpers.hpp"
#include "prime_field/primes/p64_0.hpp"

using namespace prime_field;

int main() {
    using P = P64_0;
    using Fp = FieldElement<P>;

    std::cout << "  p   = ";
    print_array(std::cout, P::p);
    std::cout << "\n\n";

    Fp a(123);
    Fp b(456);
    Fp t0 = a + b;
    Fp t1 = a * b;    
    std::cout << "  a   = " << a << "\n";
    std::cout << "  b   = " << b << "\n";
    std::cout << "a + b = " << t0 << "\n";
    std::cout << "a * b = " << t1 << "\n";
    std::cout << "\n";

    Fp c = random<P>();
    Fp d = random<P>();
    Fp t2 = c + d;
    Fp t3 = c * d;
    std::cout << "  c   = " << c << "\n";
    std::cout << "  d   = " << d << "\n";
    std::cout << "c + d = " << t2 << "\n";
    std::cout << "c * d = " << t3 << "\n";

    return 0;
}
