#pragma once

#include <string>
#include <vector>

#include <JsonDefs.h>

#include "../Defs.h"
#include "../JsonType.h"
#include "../RapidJsonHelpers.h"

namespace JsonSchemaValidator {

template <typename Type>
class JsonTypeImpl : public JsonType {
public:
	JsonTypeImpl(JsonValue const &schema, JsonResolverPtr const &resolver);

protected:
	typedef Type ValueType;

private:
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationResult &result) const;

	JsonTypeProperty<std::vector<Type>> enum_;
}; // class JsonTypeImpl : public JsonType

} // namespace JsonSchemaValidator

#include "JsonTypeImpl.inl"
