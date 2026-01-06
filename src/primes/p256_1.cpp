#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P256_1
template class FieldElement<P256_1>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P256_1>&);
template void reduce_full(FieldElement<P256_1>&);
template void add(FieldElement<P256_1>&, const FieldElement<P256_1>&, const FieldElement<P256_1>&);
template void neg(FieldElement<P256_1>&, const FieldElement<P256_1>&);
template void sub(FieldElement<P256_1>&, const FieldElement<P256_1>&, const FieldElement<P256_1>&);
template void mul(FieldElement<P256_1>&, const FieldElement<P256_1>&, const FieldElement<P256_1>&);
template void pow(FieldElement<P256_1>&, const FieldElement<P256_1>&, const std::array<digit_t, P256_1::NWORDS>&);
template void inv(FieldElement<P256_1>&, const FieldElement<P256_1>&);
template void sqrt(FieldElement<P256_1>&, const FieldElement<P256_1>&);
template int legendre(const FieldElement<P256_1>&);
template FieldElement<P256_1> random<P256_1>();
template FieldElement<P256_1> to_montgomery<P256_1>(const std::array<digit_t, P256_1::NWORDS>&);
template std::array<digit_t, P256_1::NWORDS> from_montgomery<P256_1>(const FieldElement<P256_1>&);

} // namespace prime_field
