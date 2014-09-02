#include "PrimitiveTypes.h"

#include <JsonErrors.h>

namespace JsonSchemaValidator {

template <typename Type>
bool JsonBaseNumber<Type>::CheckDivisibility(Type value, DividerType divider) {
	double division = static_cast<double>(value) / static_cast<double>(divider);
	double round_division = static_cast<double>(static_cast<long long>(division + 0.5));
	return std::fabs(division - round_division) <= std::numeric_limits<Type>::epsilon();
}

template <typename Type>
void JsonBaseNumber<Type>::CheckTypeRestrictions(JsonValue const &json,
                                                 ValidationResult &result) const {
	typename Parent::ValueType value = GetValue<typename Parent::ValueType>(json);
	if ((value < minimum_) || (exclusive_minimum_ && IsEqual(value,  minimum_))) {
		return this->RaiseError(DocumentErrors::MinimumValue, result);
	}
	if ((maximum_ < value) || (exclusive_maximum_ && IsEqual(maximum_, value))) {
		return this->RaiseError(DocumentErrors::MaximumValue, result);
	}

	if (divisible_by_.exists && !CheckDivisibility(value, divisible_by_.value)) {
		return this->RaiseError(DocumentErrors::DivisibleValue, result);
	}
}

} // namespace JsonSchemaValidator
