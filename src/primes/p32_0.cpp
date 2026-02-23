#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P32_0
template class FieldElement<P32_0>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P32_0>&);
template void reduce_full(FieldElement<P32_0>&);
template void add(FieldElement<P32_0>&, const FieldElement<P32_0>&, const FieldElement<P32_0>&);
template void neg(FieldElement<P32_0>&, const FieldElement<P32_0>&);
template void sub(FieldElement<P32_0>&, const FieldElement<P32_0>&, const FieldElement<P32_0>&);
template void mul(FieldElement<P32_0>&, const FieldElement<P32_0>&, const FieldElement<P32_0>&);
template void pow(FieldElement<P32_0>&, const FieldElement<P32_0>&, const std::array<digit_t, P32_0::NWORDS>&);
template void inv(FieldElement<P32_0>&, const FieldElement<P32_0>&);
template void sqrt(FieldElement<P32_0>&, const FieldElement<P32_0>&);
template int legendre(const FieldElement<P32_0>&);
template FieldElement<P32_0> random<P32_0>();
template void to_montgomery<P32_0>(FieldElement<P32_0>&, const std::array<digit_t, P32_0::NWORDS>&);
template void from_montgomery<P32_0>(std::array<digit_t, P32_0::NWORDS>&, const FieldElement<P32_0>&);

} // namespace prime_field
