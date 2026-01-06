#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P512_1
template class FieldElement<P512_1>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P512_1>&);
template void reduce_full(FieldElement<P512_1>&);
template void add(FieldElement<P512_1>&, const FieldElement<P512_1>&, const FieldElement<P512_1>&);
template void neg(FieldElement<P512_1>&, const FieldElement<P512_1>&);
template void sub(FieldElement<P512_1>&, const FieldElement<P512_1>&, const FieldElement<P512_1>&);
template void mul(FieldElement<P512_1>&, const FieldElement<P512_1>&, const FieldElement<P512_1>&);
template void pow(FieldElement<P512_1>&, const FieldElement<P512_1>&, const std::array<digit_t, P512_1::NWORDS>&);
template void inv(FieldElement<P512_1>&, const FieldElement<P512_1>&);
template void sqrt(FieldElement<P512_1>&, const FieldElement<P512_1>&);
template int legendre(const FieldElement<P512_1>&);
template FieldElement<P512_1> random<P512_1>();
template FieldElement<P512_1> to_montgomery<P512_1>(const std::array<digit_t, P512_1::NWORDS>&);
template std::array<digit_t, P512_1::NWORDS> from_montgomery<P512_1>(const FieldElement<P512_1>&);

} // namespace prime_field
