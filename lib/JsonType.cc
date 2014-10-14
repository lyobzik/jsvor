#include "JsonType.h"

#include <functional>
#include <algorithm>
#include <map>


#include <rapidjson/document.h>

#include "RapidJsonHelpers.h"
#include "JsonSchema.h"
#include "types/JsonTypeImpl.h"
#include "types/PrimitiveTypes.h"
#include "types/CustomTypes.h"

namespace JsonSchemaValidator {

namespace {

template <typename Type>
JsonTypePtr MakeJsonType(JsonValue const &schema, JsonResolverPtr const &resolver) {
	return std::make_shared<Type>(schema, resolver);
}

} // namespace

JsonType::JsonType(JsonValue const &schema, JsonResolverPtr const &resolver)
	: required_(false)
	, id_()
	, json_path_()
	, resolver_(resolver)
	, extends_()
	, ref_() {

	//TODO: Check $schema.
	GetChildValue(schema, "id", id_);
	GetChildValue(schema, "$ref", ref_);
	//TODO: How to detect json_path?
	GetChildValue(schema, "required", required_);

	if (!GetChildValue(schema, "extends", extends_, resolver_)) {
		JsonTypePtr extended_type;
		if (GetChildValue(schema, "extends", extended_type, resolver_)) {
			extends_.push_back(extended_type);
		}
	}
}

void JsonType::Validate(JsonValue const &json, ValidationResult &result) const {
	ValidateRef(json, result);
	if (result) {
		ValidateExtends(json, result);
	}
	if (result) {
		if (!CheckValueType(json)) {
			result.SetError(DocumentErrors::Type);
		}
	}
	if (result) {
		CheckEnumsRestrictions(json, result);
	}
	if (result) {
		CheckTypeRestrictions(json, result);
	}
}

bool JsonType::IsRequired() const
{
	return required_;
}

void JsonType::SetResolver(JsonResolverPtr const &resolver) {
	resolver_ = resolver;
}

void JsonType::ValidateExtends(JsonValue const &json, ValidationResult &result) const {
	for (auto const &json_type : extends_) {
		json_type->Validate(json, result);
	}
}

void JsonType::ValidateRef(JsonValue const &json, ValidationResult &result) const {
	if (ref_.exists && resolver_) {
		JsonSchemaPtr ref_schema = resolver_->Resolve(ref_.value);
		if (ref_schema) {
			ref_schema->Validate(json, result);
		}
	}
}

JsonTypePtr JsonType::Create(JsonValue const &schema, JsonResolverPtr const &resolver) {
	if (!schema.IsObject()) {
		RaiseError(SchemaErrors::NotJsonObject);
	}

	JsonValueMember const *type = FindMember(schema, "type");
	JsonTypeCreator creator = type ? GetCreator(type->value) : MakeJsonType<JsonAny>;
	if (!creator) {
		RaiseError(SchemaErrors::CantDetectType);
	}

	return creator(schema, resolver);
}

JsonTypeCreator JsonType::GetCreator(JsonValue const &type) {
	if (type.IsString()) {
		static std::map<std::string, JsonTypeCreator> creators{
			{ "string", MakeJsonType<JsonString> },
			{ "number", MakeJsonType<JsonNumber> },
			{ "integer", MakeJsonType<JsonInteger> },
			{ "boolean", MakeJsonType<JsonBoolean> },
			{ "object", MakeJsonType<JsonObject> },
			{ "array", MakeJsonType<JsonArray> },
			{ "null", MakeJsonType<JsonNull> },
			{ "any", MakeJsonType<JsonAny> }
		};

		auto creator = creators.find(GetValue<char const *>(type));
		if (creator != creators.end()) {
			return creator->second;
		}
		return MakeJsonType<JsonAny>;
	}
	if (type.IsObject()) {
		return MakeJsonType<JsonCustomType>;
	}
	if (type.IsArray()) {
		return MakeJsonType<JsonUnionType>;
	}
	return JsonTypeCreator();
}

JsonTypePtr JsonType::CreateJsonTypeFromArrayElement(JsonValue const &schema,
	                                                 JsonResolverPtr const &resolver) {
	if (schema.IsObject()) {
		return JsonType::Create(schema, resolver);
	}
	if (schema.IsString()) {
		JsonTypeCreator creator = GetCreator(schema);
		return creator((JsonValue()), resolver);
	}
	RaiseError(SchemaErrors::IncorrectType);
	return JsonTypePtr();
}

void JsonType::RaiseError(DocumentErrors error, ValidationResult &result) {
	result.SetError(error);  //TODO: Add json_path
}

void JsonType::RaiseError(SchemaErrors error) {
	throw IncorrectSchema(error);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
bool GetValue<JsonTypePtr>(JsonValue const &json, JsonTypePtr &value) {
	if (json.IsObject()) {
		value = JsonType::Create(json, JsonResolverPtr());
		return true;
	}
	return false;
}

template <>
bool GetValue<std::vector<JsonTypePtr>>(JsonValue const &json, std::vector<JsonTypePtr> &value) {
	if (json.IsArray()) {
		for (JsonSizeType i = 0; i < json.Size(); ++i) {
			value.push_back(JsonType::Create(json[i], JsonResolverPtr()));
		}
		return true;
	}
	return false;
}

} // namespace JsonSchemaValidator
