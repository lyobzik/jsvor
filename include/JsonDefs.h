#pragma once

#include <memory>

#include <rapidjson/document.h>

namespace JsonSchemaValidator {

class JsonSchema;
typedef std::shared_ptr<JsonSchema> JsonSchemaPtr;

class JsonResolver;
typedef std::shared_ptr<JsonResolver> JsonResolverPtr;

class ValidationResult;

typedef rapidjson::Value JsonValue;
typedef rapidjson::Document JsonDocument;

} // namespace JsonSchemaValidator
