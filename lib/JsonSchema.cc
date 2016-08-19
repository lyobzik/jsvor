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

#include "../include/JsonSchema.h"

#include <memory>
#include <string>

#include "../include/JsonResolver.h"

#include "JsonType.h"
#include "ValidationContext.h"

namespace JsonSchemaValidator {

namespace {

#include "CoreSchema.inl"

class CoreSchemaResolver
{
public:
	CoreSchemaResolver() = default;
	virtual ~CoreSchemaResolver() = default;

	virtual JsonSchemaPtr Resolve(std::string const &ref) const {
		return ref == "#" ? core_schema_ : JsonSchemaPtr();
	}

	void SetCoreSchema(JsonSchemaPtr const& core_schema) {
		core_schema_ = core_schema;
	}

private:
	JsonSchemaPtr core_schema_;
};

JsonSchemaPtr CreateCoreSchema() {
	auto resolver = std::make_shared<CoreSchemaResolver>();
	auto schema = std::make_shared<JsonSchema>(core_schema_draft03_desc,
	                                           std::dynamic_pointer_cast<JsonResolver>(resolver));
	resolver->SetCoreSchema(schema);
	return schema;
}

static JsonSchemaPtr core_schema_draft03 = CreateCoreSchema();

void Parse(char const *document, JsonDocument &json) {
	json.Parse<0>(document);

	if (json.HasParseError()) {
		throw IncorrectJson(GetLastError(json));
	}
}

template <typename Document>
void Validate(JsonSchema const &schema, Document const &document) {
	ValidationResult result;
	schema.Validate(document, result);
	if (!result) {
		throw IncorrectDocument(std::move(result));
	}
}

} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
class SimpleResolver : public JsonResolver {
public:
	SimpleResolver();
	virtual ~SimpleResolver();

	virtual JsonSchemaPtr Resolve(std::string const &ref) const;
};

SimpleResolver::SimpleResolver() {
}

SimpleResolver::~SimpleResolver() {
}

JsonSchemaPtr SimpleResolver::Resolve(std::string const &/*ref*/) const {
	//TODO: Implement resolving of simple links.
	return JsonSchemaPtr();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
struct JsonSchema::Impl {
	JsonDocument schema_document_;

	JsonResolverPtr resolver_;
	JsonTypePtr root_object_;

	Impl();
}; // struct JsonSchema::Impl

JsonSchema::Impl::Impl()
	: schema_document_()
	, resolver_(std::make_shared<SimpleResolver>())
	, root_object_() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonSchema::JsonSchema(char const *schema, JsonResolverPtr const &resolver)
	: impl_(std::make_shared<Impl>()) {

	Parse(schema, impl_->schema_document_);
	Initialize(impl_->schema_document_, resolver);
}

JsonSchema::JsonSchema(std::string const &schema, JsonResolverPtr const &resolver)
	: impl_(std::make_shared<Impl>()) {

	Parse(schema.c_str(), impl_->schema_document_);
	Initialize(impl_->schema_document_, resolver);
}

JsonSchema::JsonSchema(JsonValue const &schema, JsonResolverPtr const &resolver)
	: impl_(std::make_shared<Impl>()) {

	Initialize(schema, resolver);
}

void JsonSchema::Validate(char const *document) const {
	JsonSchemaValidator::Validate(*this, document);
}

void JsonSchema::Validate(std::string const &document) const {
	JsonSchemaValidator::Validate(*this, document);
}

void JsonSchema::Validate(JsonValue const &document) const {
	JsonSchemaValidator::Validate(*this, document);
}

void JsonSchema::Validate(char const *document, ValidationResult &result) const {
	rapidjson::Document inspected_document;

	Parse(document, inspected_document);
	Validate(inspected_document, result);
}

void JsonSchema::Validate(std::string const &document, ValidationResult &result) const {
	Validate(document.c_str(), result);
}

void JsonSchema::Validate(JsonValue const &document, ValidationResult &result) const {
	ValidationContext context(result);
	Validate(document, context);
}

void JsonSchema::Validate(JsonValue const &document, ValidationContext &context) const {
	impl_->root_object_->Validate(document, context);
}

void JsonSchema::Initialize(JsonValue const &schema, JsonResolverPtr const &resolver) {
	if ((!schema.HasMember("$schema") ||
	     (schema.HasMember("$schema") && schema["$schema"].IsString() &&
	      (schema["$schema"] == "http://json-schema.org/draft-03/schema#" ||
	       schema["$schema"] == "http://json-schema.org/draft-03/schema"))) &&
	    core_schema_draft03) {

		core_schema_draft03->Validate(schema);
	}
	if (resolver) {
		impl_->resolver_ = resolver;
	}

	impl_->root_object_ = JsonType::Create(schema, impl_->resolver_, "/");
}

} // namespace JsonSchemaValidator
