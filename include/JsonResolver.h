#pragma once

#include <string>

#include "JsonDefs.h"

namespace JsonSchemaValidator {

// Resolver for resolving links in json-schema.
class JsonResolver {
public:
	JsonResolver();
	virtual ~JsonResolver();

	virtual JsonSchemaPtr Resolve(std::string const &ref) const = 0;
}; // class JsonResolver

} // namespace JsonSchemaValidator
