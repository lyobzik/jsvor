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
