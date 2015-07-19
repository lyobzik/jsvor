#include "ValidationContext.h"

#include <string>

namespace JsonSchemaValidator {

ValidationContext::ValidationContext(ValidationResult &result)
	: result_(result) {
}

ValidationContext::~ValidationContext() {
}

ValidationResult& ValidationContext::GetResult() {
	return result_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
std::string PathHolder<char const *>::ElementNameToString() const {
	return name_;
}

template <>
std::string PathHolder<rapidjson::SizeType>::ElementNameToString() const {
	return "[" + std::to_string(name_) + "]";
}

} // namespace JsonSchemaValidator
