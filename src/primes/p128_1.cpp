#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P128_1
template class FieldElement<P128_1>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P128_1>&);
template void reduce_full(FieldElement<P128_1>&);
template void add(FieldElement<P128_1>&, const FieldElement<P128_1>&, const FieldElement<P128_1>&);
template void neg(FieldElement<P128_1>&, const FieldElement<P128_1>&);
template void sub(FieldElement<P128_1>&, const FieldElement<P128_1>&, const FieldElement<P128_1>&);
template void mul(FieldElement<P128_1>&, const FieldElement<P128_1>&, const FieldElement<P128_1>&);
template void pow(FieldElement<P128_1>&, const FieldElement<P128_1>&, const std::array<digit_t, P128_1::NWORDS>&);
template void inv(FieldElement<P128_1>&, const FieldElement<P128_1>&);
template void sqrt(FieldElement<P128_1>&, const FieldElement<P128_1>&);
template int legendre(const FieldElement<P128_1>&);
template FieldElement<P128_1> random<P128_1>();
template FieldElement<P128_1> to_montgomery<P128_1>(const std::array<digit_t, P128_1::NWORDS>&);
template std::array<digit_t, P128_1::NWORDS> from_montgomery<P128_1>(const FieldElement<P128_1>&);

} // namespace prime_field
