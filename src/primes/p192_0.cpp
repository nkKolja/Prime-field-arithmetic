#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P192_0
template class FieldElement<P192_0>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P192_0>&);
template void reduce_full(FieldElement<P192_0>&);
template void add(FieldElement<P192_0>&, const FieldElement<P192_0>&, const FieldElement<P192_0>&);
template void neg(FieldElement<P192_0>&, const FieldElement<P192_0>&);
template void sub(FieldElement<P192_0>&, const FieldElement<P192_0>&, const FieldElement<P192_0>&);
template void mul(FieldElement<P192_0>&, const FieldElement<P192_0>&, const FieldElement<P192_0>&);
template void pow(FieldElement<P192_0>&, const FieldElement<P192_0>&, const std::array<digit_t, P192_0::NWORDS>&);
template void inv(FieldElement<P192_0>&, const FieldElement<P192_0>&);
template void sqrt(FieldElement<P192_0>&, const FieldElement<P192_0>&);
template int legendre(const FieldElement<P192_0>&);
template FieldElement<P192_0> random<P192_0>();
template FieldElement<P192_0> to_montgomery<P192_0>(const std::array<digit_t, P192_0::NWORDS>&);
template std::array<digit_t, P192_0::NWORDS> from_montgomery<P192_0>(const FieldElement<P192_0>&);

} // namespace prime_field
