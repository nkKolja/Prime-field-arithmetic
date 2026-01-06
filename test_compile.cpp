#include <iostream>
#include "prime_field.hpp"

int main() {
    using namespace prime_field;
    
    FieldElement<P64_0> a(5);
    std::cout << "Success!" << std::endl;
    
    return 0;
}
