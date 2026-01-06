#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P512_0
template class FieldElement<P512_0>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P512_0>&);
template void reduce_full(FieldElement<P512_0>&);
template void add(FieldElement<P512_0>&, const FieldElement<P512_0>&, const FieldElement<P512_0>&);
template void neg(FieldElement<P512_0>&, const FieldElement<P512_0>&);
template void sub(FieldElement<P512_0>&, const FieldElement<P512_0>&, const FieldElement<P512_0>&);
template void mul(FieldElement<P512_0>&, const FieldElement<P512_0>&, const FieldElement<P512_0>&);
template void pow(FieldElement<P512_0>&, const FieldElement<P512_0>&, const std::array<digit_t, P512_0::NWORDS>&);
template void inv(FieldElement<P512_0>&, const FieldElement<P512_0>&);
template void sqrt(FieldElement<P512_0>&, const FieldElement<P512_0>&);
template int legendre(const FieldElement<P512_0>&);
template FieldElement<P512_0> random<P512_0>();
template FieldElement<P512_0> to_montgomery<P512_0>(const std::array<digit_t, P512_0::NWORDS>&);
template std::array<digit_t, P512_0::NWORDS> from_montgomery<P512_0>(const FieldElement<P512_0>&);

} // namespace prime_field
