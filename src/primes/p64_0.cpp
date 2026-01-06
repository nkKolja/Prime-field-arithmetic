#include "../../include/prime_field.hpp"

using namespace prime_field::montgomery;

namespace prime_field {

// Explicit template instantiation for P64_0
template class FieldElement<P64_0>;

// Explicitly instantiate all operations
template void reduce(FieldElement<P64_0>&);
template void reduce_full(FieldElement<P64_0>&);
template void add(FieldElement<P64_0>&, const FieldElement<P64_0>&, const FieldElement<P64_0>&);
template void neg(FieldElement<P64_0>&, const FieldElement<P64_0>&);
template void sub(FieldElement<P64_0>&, const FieldElement<P64_0>&, const FieldElement<P64_0>&);
template void mul(FieldElement<P64_0>&, const FieldElement<P64_0>&, const FieldElement<P64_0>&);
template void pow(FieldElement<P64_0>&, const FieldElement<P64_0>&, const std::array<digit_t, P64_0::NWORDS>&);
template void inv(FieldElement<P64_0>&, const FieldElement<P64_0>&);
template void sqrt(FieldElement<P64_0>&, const FieldElement<P64_0>&);
template int legendre(const FieldElement<P64_0>&);
template FieldElement<P64_0> random<P64_0>();
template FieldElement<P64_0> to_montgomery<P64_0>(const std::array<digit_t, P64_0::NWORDS>&);
template std::array<digit_t, P64_0::NWORDS> from_montgomery<P64_0>(const FieldElement<P64_0>&);

} // namespace prime_field
