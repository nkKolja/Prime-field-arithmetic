#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P128_0
template class FieldElement<P128_0>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P128_0>&);
template void reduce_full(FieldElement<P128_0>&);
template void add(FieldElement<P128_0>&, const FieldElement<P128_0>&, const FieldElement<P128_0>&);
template void neg(FieldElement<P128_0>&, const FieldElement<P128_0>&);
template void sub(FieldElement<P128_0>&, const FieldElement<P128_0>&, const FieldElement<P128_0>&);
template void mul(FieldElement<P128_0>&, const FieldElement<P128_0>&, const FieldElement<P128_0>&);
template void pow(FieldElement<P128_0>&, const FieldElement<P128_0>&, const std::array<digit_t, P128_0::NWORDS>&);
template void inv(FieldElement<P128_0>&, const FieldElement<P128_0>&);
template void sqrt(FieldElement<P128_0>&, const FieldElement<P128_0>&);
template int legendre(const FieldElement<P128_0>&);
template FieldElement<P128_0> random<P128_0>();
template FieldElement<P128_0> to_montgomery<P128_0>(const std::array<digit_t, P128_0::NWORDS>&);
template std::array<digit_t, P128_0::NWORDS> from_montgomery<P128_0>(const FieldElement<P128_0>&);

} // namespace prime_field
