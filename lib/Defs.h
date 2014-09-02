#pragma once

#include <memory>
#include <functional>

#include "RapidJsonDefs.h"
#include "../include/JsonDefs.h"

namespace JsonSchemaValidator {

class ValidationResult;

class JsonType;
typedef std::shared_ptr<JsonType> JsonTypePtr;

typedef std::function<JsonTypePtr(JsonValue const&, JsonResolverPtr const &)> JsonTypeCreator;

class Regex;
typedef std::shared_ptr<Regex> RegexPtr;

} // namespace JsonSchemaValidator
