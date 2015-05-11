#include "CustomTypes.h"

#include <JsonSchema.h>
#include <JsonResolver.h>

#include "../Regex.h"

#include "PrimitiveTypes.h"

namespace JsonSchemaValidator {

JsonCustomType::JsonCustomType(JsonValue const &schema, JsonResolverPtr const &resolver)
	: JsonType(schema, resolver)
	, custom_type_() {

	JsonValueMember const *type = FindMember(schema, "type");
	if (!type) {
		RaiseError(SchemaErrors::IncorrectCustomType);
	}
	custom_type_ = JsonType::Create(type->value, resolver);
}

bool JsonCustomType::CheckValueType(JsonValue const &/*json*/) const {
	return true;
}

void JsonCustomType::CheckTypeRestrictions(JsonValue const &json, ValidationResult &result) const {
	return custom_type_->Validate(json, result);
}

void JsonCustomType::CheckEnumsRestrictions(JsonValue const &/*json*/,
                                            ValidationResult &/*result*/) const {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonAny::JsonAny(JsonValue const &schema, JsonResolverPtr const &resolver)
	: JsonType(schema, resolver)
	, disallow_()
	, string_(std::make_shared<JsonString>(schema, resolver))
	, number_(std::make_shared<JsonNumber>(schema, resolver))
	, integer_(std::make_shared<JsonInteger>(schema, resolver))
	, boolean_(std::make_shared<JsonBoolean>(schema, resolver))
	, object_(std::make_shared<JsonObject>(schema, resolver))
	, array_(std::make_shared<JsonArray>(schema, resolver))
	, null_(std::make_shared<JsonNull>(schema, resolver)) {

	JsonValueMember const *disallow = FindMember(schema, "disallow");
	if (disallow) {
		if (disallow->value.IsArray()) {
			for (JsonSizeType i = 0; i < disallow->value.Size(); ++i) {
				JsonTypePtr json_type = CreateJsonTypeFromArrayElement(disallow->value[i], resolver);
				disallow_.insert(json_type);
			}
		}
		else if (disallow->value.IsObject() || disallow->value.IsString()) {
			JsonTypePtr json_type = CreateJsonTypeFromArrayElement(disallow->value, resolver);
			disallow_.insert(json_type);
		}
		else {
			RaiseError(SchemaErrors::IncorrectDisallowType);
		}
	}
}

void JsonAny::Validate(JsonValue const &json, ValidationResult &result) const {
	for (auto const &disallow : disallow_) {
		ValidationResult disallow_result;
		disallow->Validate(json, disallow_result);
		if (disallow_result) {
			return RaiseError(DocumentErrors::DisallowType,
			                  "Satisfies one of disallow type.", json, result);
		}
	}
	GetSchema(json)->Validate(json, result);
}

bool JsonAny::CheckValueType(JsonValue const &/*json*/) const {
	return true;
}

void JsonAny::CheckTypeRestrictions(JsonValue const &/*json*/, ValidationResult &/*res*/) const {
}

void JsonAny::CheckEnumsRestrictions(JsonValue const &/*json*/, ValidationResult &/*res*/) const {
}

JsonTypePtr const &JsonAny::GetSchema(JsonValue const &json) const {
	if (json.IsObject()) return object_;
	if (json.IsArray())  return array_;
	if (json.IsString()) return string_;
	if (json.IsDouble()) return number_;
	if (json.IsNumber()) return integer_;
	if (json.IsBool())   return boolean_;
	return null_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonUnionType::JsonUnionType(JsonValue const &schema, JsonResolverPtr const &resolver)
	: JsonType(schema, resolver)
	, type_() {

	JsonValueMember const *type = FindMember(schema, "type");
	if (!type || !(type->value.IsArray() || type->value.IsObject())) {
		RaiseError(SchemaErrors::IncorrectUnionType);
	}
	if (type->value.IsArray()) {
		for (JsonSizeType i = 0; i < type->value.Size(); ++i) {
			JsonTypePtr json_type = CreateJsonTypeFromArrayElement(type->value[i], resolver);
			type_.insert(json_type);
		}
	}
	else if (type->value.IsObject()) {
		JsonTypeCreator creator = GetCreator(type->value);
		type_.insert(creator(schema, resolver));
	}
}

void JsonUnionType::Validate(JsonValue const &json, ValidationResult &result) const {
	for (auto const &type : type_) {
		ValidationResult type_result;
		type->Validate(json, type_result);
		if (type_result) {
			return;
		}
	}
	return RaiseError(DocumentErrors::NeitherType,
	                  "Doesn't satisfy any allow type.", json, result);
}

bool JsonUnionType::CheckValueType(JsonValue const &/*json*/) const {
	return true;
}

void JsonUnionType::CheckTypeRestrictions(JsonValue const &/*json*/,
                                          ValidationResult &/*result*/) const {
}

void JsonUnionType::CheckEnumsRestrictions(JsonValue const &/*json*/,
                                           ValidationResult &/*result*/) const {
}

} // namespace JsonSchemaValidator
