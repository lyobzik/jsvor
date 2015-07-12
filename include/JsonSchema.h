#pragma once

#include <string>
#include <memory>

#include "JsonDefs.h"
#include "JsonErrors.h"

namespace JsonSchemaValidator {

class ValidationContext;

// Json-schema for validating json-documents.
class JsonSchema {
public:
	explicit JsonSchema(char const *schema, JsonResolverPtr const &resolver = nullptr);
	explicit JsonSchema(std::string const &schema, JsonResolverPtr const &resolver = nullptr);
	// Object used as argument 'schema' must live longer than created JsonSchema.
	explicit JsonSchema(JsonValue const &schema, JsonResolverPtr const &resolver = nullptr);

	// Validate document and throw exception on validation error.
	void Validate(char const *document) const;
	void Validate(std::string const &document) const;
	void Validate(JsonValue const &document) const;

	// Validate document and return result in 'result' parameter.
	void Validate(char const *document, ValidationResult &result) const;
	void Validate(std::string const &document, ValidationResult &result) const;
	void Validate(JsonValue const &document, ValidationResult &result) const;

private:
	friend class JsonType;

	void Validate(JsonValue const &document, ValidationContext &context) const;
	void Initialize(JsonValue const &schema, JsonResolverPtr const &resolver);

	struct Impl;

	std::shared_ptr<Impl> impl_;
}; // class JsonSchema

} // namespace JsonSchemaValidator
