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

#include <JsonSchema.h>
#include <JsonResolver.h>
#include <JsonErrors.h>

#include "../Regex.h"
#include "../ValidationContext.h"

namespace JsonSchemaValidator {

namespace {

template <typename Type>
struct JsonChecker {
	static bool ContainsValue(std::vector<Type> const &values, Type const &value) {
		return std::binary_search(values.begin(), values.end(), value);
	}
}; // class JsonChecker

struct JsonCheckerNotOrdered {
	template <typename Type>
	static bool ContainsValue(std::vector<Type> const &values, Type const &value) {
		auto const it = std::find_if(begin(values), end(values), [&value](Type const &elem) {
			return IsEqual(value.GetValue(), elem.GetValue());
		});
		return it != end(values);
	}
}; // struct JsonCheckerNotOrdered

template <>
struct JsonChecker<JsonObjectValue> : public JsonCheckerNotOrdered {
}; // class JsonChecker<JsonObjectValue>

template <>
struct JsonChecker<JsonArrayValue> : public JsonCheckerNotOrdered {
}; // class JsonChecker<JsonArrayValue>

template <>
struct JsonChecker<JsonNullValue> : public JsonCheckerNotOrdered {
}; // class JsonChecker<JsonNullValue>

} // namespace

template <typename Type>
JsonTypeImpl<Type>::JsonTypeImpl(JsonValue const &schema, JsonResolverPtr const &resolver,
                                 std::string const &path)
	: JsonType(schema, resolver, path)
	, enum_() {

	GetChildValue(schema, "enum", enum_);
}

template <typename Type>
void JsonTypeImpl<Type>::CheckEnumsRestrictions(JsonValue const &json,
                                                ValidationContext &context) const {
	if (enum_.exists) {
		Type value = GetValue<Type>(json);
		if (!JsonChecker<Type>::ContainsValue(enum_.value, value)) {
			return RaiseError<EnumValueError>(context);
		}
	}
}

} // namespace JsonSchemaValidator
