#include "../include/JsonSchema.h"

#include <memory>

#include "../include/JsonResolver.h"

#include "JsonType.h"

namespace JsonSchemaValidator {

namespace {

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
		throw IncorrectDocument(result);
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
	: JsonSchema(schema.c_str(), resolver) {
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
	impl_->root_object_->Validate(document, result);
}

void JsonSchema::Initialize(JsonValue const &schema, JsonResolverPtr const &resolver) {
	if (resolver) {
		impl_->resolver_ = resolver;
	}

	impl_->root_object_ = JsonType::Create(schema, impl_->resolver_);
}

} // namespace JsonSchemaValidator
