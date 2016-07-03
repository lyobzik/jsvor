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

#include "CustomTypes.h"

#include <JsonSchema.h>
#include <JsonResolver.h>

#include "../Regex.h"
#include "../ValidationContext.h"

#include "PrimitiveTypes.h"

namespace JsonSchemaValidator {

JsonCustomType::JsonCustomType(JsonValue const &schema, JsonResolverPtr const &resolver,
                               std::string const &path)
	: JsonType(schema, resolver, path)
	, custom_type_() {

	JsonValueMember const *type = FindMember(schema, "type");
	if (!type) {
		RaiseError(SchemaErrors::IncorrectCustomType);
	}
	custom_type_ = JsonType::Create(type->value, resolver, path);
}

bool JsonCustomType::CheckValueType(JsonValue const &/*json*/) const {
	return true;
}

void JsonCustomType::CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const {
	return custom_type_->Validate(json, context);
}

void JsonCustomType::CheckEnumsRestrictions(JsonValue const &/*json*/,
                                            ValidationContext &/*context*/) const {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonAny::JsonAny(JsonValue const &schema, JsonResolverPtr const &resolver,
                 std::string const &path)
	: JsonType(schema, resolver, path)
	, disallow_()
	, string_(std::make_shared<JsonString>(schema, resolver, path))
	, number_(std::make_shared<JsonNumber>(schema, resolver, path))
	, integer_(std::make_shared<JsonInteger>(schema, resolver, path))
	, boolean_(std::make_shared<JsonBoolean>(schema, resolver, path))
	, object_(std::make_shared<JsonObject>(schema, resolver, path))
	, array_(std::make_shared<JsonArray>(schema, resolver, path))
	, null_(std::make_shared<JsonNull>(schema, resolver, path)) {

	JsonValueMember const *disallow = FindMember(schema, "disallow");
	if (disallow) {
		if (disallow->value.IsArray()) {
			for (JsonSizeType i = 0; i < disallow->value.Size(); ++i) {
				JsonTypePtr json_type = CreateJsonTypeFromArrayElement(disallow->value[i],
				                                                       resolver, path);
				disallow_.insert(json_type);
			}
		}
		else if (disallow->value.IsObject() || disallow->value.IsString()) {
			JsonTypePtr json_type = CreateJsonTypeFromArrayElement(disallow->value,
			                                                       resolver, path);
			disallow_.insert(json_type);
		}
		else {
			RaiseError(SchemaErrors::IncorrectDisallowType);
		}
	}
}

void JsonAny::Validate(JsonValue const &json, ValidationContext &context) const {
	for (auto const &disallow : disallow_) {
		ValidationResult disallow_result;
		ValidationContext disallow_context(disallow_result);
		disallow->Validate(json, disallow_context);
		if (disallow_result) {
			return RaiseError<DisallowTypeError>(context);
		}
	}
	GetSchema(json)->Validate(json, context);
}

bool JsonAny::CheckValueType(JsonValue const &/*json*/) const {
	return true;
}

void JsonAny::CheckTypeRestrictions(JsonValue const &/*json*/,
                                    ValidationContext &/*context*/) const {
}

void JsonAny::CheckEnumsRestrictions(JsonValue const &/*json*/,
                                     ValidationContext &/*context*/) const {
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
JsonUnionType::JsonUnionType(JsonValue const &schema, JsonResolverPtr const &resolver,
                             std::string const &path)
	: JsonType(schema, resolver, path)
	, type_() {

	JsonValueMember const *type = FindMember(schema, "type");
	if (!type || !(type->value.IsArray() || type->value.IsObject())) {
		RaiseError(SchemaErrors::IncorrectUnionType);
	}
	if (type->value.IsArray()) {
		for (JsonSizeType i = 0; i < type->value.Size(); ++i) {
			JsonTypePtr json_type = CreateJsonTypeFromArrayElement(type->value[i], resolver, path);
			type_.insert(json_type);
		}
	}
	else if (type->value.IsObject()) {
		JsonTypeCreator creator = GetCreator(type->value);
		type_.insert(creator(schema, resolver, path));
	}
}

void JsonUnionType::Validate(JsonValue const &json, ValidationContext &context) const {
	for (auto const &type : type_) {
		ValidationResult type_result;
		ValidationContext type_context(type_result);
		type->Validate(json, type_context);
		if (type_result) {
			return;
		}
	}
	return RaiseError<NeitherTypeError>(context); // TODO: Specify all child errors.
}

bool JsonUnionType::CheckValueType(JsonValue const &/*json*/) const {
	return true;
}

void JsonUnionType::CheckTypeRestrictions(JsonValue const &/*json*/,
                                          ValidationContext &/*context*/) const {
}

void JsonUnionType::CheckEnumsRestrictions(JsonValue const &/*json*/,
                                           ValidationContext &/*context*/) const {
}

} // namespace JsonSchemaValidator
