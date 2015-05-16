#include <JsonSchema.h>
#include <JsonResolver.h>
#include <JsonErrors.h>

#include "../Regex.h"

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
                                                ValidationResult &result) const {
	if (enum_.exists) {
		Type value = GetValue<Type>(json);
		if (!JsonChecker<Type>::ContainsValue(enum_.value, value)) {
			return RaiseError(DocumentErrors::EnumValue,
			                  ToString("Must be one of ", /*enum_.value,*/ "."),
			                  result);
		}
	}
}

} // namespace JsonSchemaValidator
