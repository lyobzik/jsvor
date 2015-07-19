#include "JsonType.h"

#include <functional>
#include <algorithm>
#include <map>


#include <rapidjson/document.h>

#include "RapidJsonHelpers.h"
#include "JsonSchema.h"
#include "ValidationContext.h"
#include "types/JsonTypeImpl.h"
#include "types/PrimitiveTypes.h"
#include "types/CustomTypes.h"

namespace JsonSchemaValidator {

namespace {

template <typename Type>
JsonTypePtr MakeJsonType(JsonValue const &schema, JsonResolverPtr const &resolver,
                         std::string const &path) {
	return std::make_shared<Type>(schema, resolver, path);
}

} // namespace

JsonType::JsonType(JsonValue const &schema, JsonResolverPtr const &resolver,
                   std::string const &path)
	: required_(false)
	, id_()
	, path_(path)
	, resolver_(resolver)
	, extends_()
	, ref_() {

	//TODO: Check $schema.
	GetChildValue(schema, "id", id_);
	GetChildValue(schema, "$ref", ref_);
	GetChildValue(schema, "required", required_);

	if (!GetChildValue(schema, "extends", extends_, resolver_, path_)) {
		JsonTypePtr extended_type;
		if (GetChildValue(schema, "extends", extended_type, resolver_, path_)) {
			extends_.push_back(extended_type);
		}
	}
}

void JsonType::Validate(JsonValue const &json, ValidationContext &context) const {
	ValidateRef(json, context);
	if (context.GetResult()) {
		ValidateExtends(json, context);
	}
	if (context.GetResult()) {
		if (!CheckValueType(json)) {
			RaiseError<TypeError>(context); //TODO: Specify required type
		}
	}
	if (context.GetResult()) {
		CheckEnumsRestrictions(json, context);
	}
	if (context.GetResult()) {
		CheckTypeRestrictions(json, context);
	}
}

bool JsonType::IsRequired() const
{
	return required_;
}

void JsonType::ValidateExtends(JsonValue const &json, ValidationContext &context) const {
	for (auto const &json_type : extends_) {
		json_type->Validate(json, context);
	}
}

void JsonType::ValidateRef(JsonValue const &json, ValidationContext &context) const {
	if (ref_.exists && resolver_) {
		JsonSchemaPtr ref_schema = resolver_->Resolve(ref_.value);
		if (ref_schema) {
			ref_schema->Validate(json, context);
		}
	}
}

JsonTypePtr JsonType::Create(JsonValue const &schema, JsonResolverPtr const &resolver,
                             std::string const &path) {
	if (!schema.IsObject()) {
		RaiseError(SchemaErrors::NotJsonObject);
	}

	JsonValueMember const *type = FindMember(schema, "type");
	JsonTypeCreator creator = type ? GetCreator(type->value) : MakeJsonType<JsonAny>;
	if (!creator) {
		RaiseError(SchemaErrors::CantDetectType);
	}

	return creator(schema, resolver, path);
}

std::string JsonType::MemberPath(char const *member) const {
	if (!path_.empty() && path_[path_.size() - 1] == '/') {
		return path_ + member;
	}
	return (path_ + "/") + member;
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
	                                                 JsonResolverPtr const &resolver,
                                                     std::string const &path) {
	if (schema.IsObject()) {
		return JsonType::Create(schema, resolver, path);
	}
	if (schema.IsString()) {
		JsonTypeCreator creator = GetCreator(schema);
		return creator((JsonValue()), resolver, path);
	}
	RaiseError(SchemaErrors::IncorrectType);
	return JsonTypePtr();
}

void JsonType::RaiseError(SchemaErrors error) {
	throw IncorrectSchema(error);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool GetValue(JsonValue const &json, JsonTypePtr &value, JsonResolverPtr const &resolver,
              std::string const &path) {
	if (json.IsObject()) {
		value = JsonType::Create(json, resolver, path);
		return true;
	}
	return false;
}

bool GetValue(JsonValue const &json, std::vector<JsonTypePtr> &value,
              JsonResolverPtr const &resolver, std::string const &path) {
	if (json.IsArray()) {
		for (JsonSizeType i = 0; i < json.Size(); ++i) {
			value.push_back(JsonType::Create(json[i], resolver, path));
		}
		return true;
	}
	return false;
}

} // namespace JsonSchemaValidator
