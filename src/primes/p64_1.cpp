#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P64_1
template class FieldElement<P64_1>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P64_1>&);
template void reduce_full(FieldElement<P64_1>&);
template void add(FieldElement<P64_1>&, const FieldElement<P64_1>&, const FieldElement<P64_1>&);
template void neg(FieldElement<P64_1>&, const FieldElement<P64_1>&);
template void sub(FieldElement<P64_1>&, const FieldElement<P64_1>&, const FieldElement<P64_1>&);
template void mul(FieldElement<P64_1>&, const FieldElement<P64_1>&, const FieldElement<P64_1>&);
template void pow(FieldElement<P64_1>&, const FieldElement<P64_1>&, const std::array<digit_t, P64_1::NWORDS>&);
template void inv(FieldElement<P64_1>&, const FieldElement<P64_1>&);
template void sqrt(FieldElement<P64_1>&, const FieldElement<P64_1>&);
template int legendre(const FieldElement<P64_1>&);
template FieldElement<P64_1> random<P64_1>();
template FieldElement<P64_1> to_montgomery<P64_1>(const std::array<digit_t, P64_1::NWORDS>&);
template std::array<digit_t, P64_1::NWORDS> from_montgomery<P64_1>(const FieldElement<P64_1>&);

} // namespace prime_field
