#pragma once

#include <vector>
#include <string>

#include "Defs.h"

namespace JsonSchemaValidator {

class ValidationContext {
public:
	explicit ValidationContext(ValidationResult &result);

	void AddPath(std::string const &path);
	void RemovePath();

	ValidationResult& GetResult();
	std::string GetPath(std::string const &path) const;
private:
	std::vector<std::string> path_parts_;
	ValidationResult &result_;
}; // class ValidationContext

class ResolvePathHolder {
public:
	ResolvePathHolder(std::string const &path, ValidationContext &context);
	~ResolvePathHolder();

private:
	ValidationContext &context_;
}; // class ResolvePathContext

} // namespace JsonSchemaValidator
