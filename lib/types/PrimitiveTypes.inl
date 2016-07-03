// Copyright 2016 lyobzik
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "PrimitiveTypes.h"

#include <JsonErrors.h>

#include "../ValidationContext.h"

namespace JsonSchemaValidator {

template <typename Type>
bool JsonBaseNumber<Type>::CheckDivisibility(Type value, DividerType divider) {
	double division = static_cast<double>(value) / static_cast<double>(divider);
	double round_division = static_cast<double>(static_cast<long long>(division + 0.5));
	return std::fabs(division - round_division) <= std::numeric_limits<Type>::epsilon();
}

template <typename Type>
void JsonBaseNumber<Type>::CheckTypeRestrictions(JsonValue const &json,
                                                 ValidationContext &context) const {
	typename Parent::ValueType value = GetValue<typename Parent::ValueType>(json);
	if ((value < minimum_) || (exclusive_minimum_ && IsEqual(value,  minimum_))) {
		return this->template RaiseError<MinimumValueError<Type> >(context, minimum_,
		                                                           exclusive_minimum_);
	}
	if ((maximum_ < value) || (exclusive_maximum_ && IsEqual(maximum_, value))) {
		return this->template RaiseError<MaximumValueError<Type> >(context, maximum_,
		                                                           exclusive_maximum_);
	}

	if (divisible_by_.exists && !CheckDivisibility(value, divisible_by_.value)) {
		return this->template RaiseError<DivisibleValueError>(context, divisible_by_.value);
	}
}

} // namespace JsonSchemaValidator
