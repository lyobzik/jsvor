#pragma once

#include "Defs.h"

namespace JsonSchemaValidator {

class ValidationContext {
public:
	explicit ValidationContext(ValidationResult &result);
	~ValidationContext();

	ValidationResult& GetResult();
private:
	ValidationResult &result_;
}; // class ValidationContext

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename ElementName>
class PathHolder {
public:
	PathHolder(ElementName const &name, ValidationContext &context);
	~PathHolder();

	void Reset();

private:
	std::string ElementNameToString() const;

	bool need_set_;
	ElementName name_;
	ValidationContext &context_;
}; // class PathHolder

typedef PathHolder<char const *> MemberPathHolder;
typedef PathHolder<rapidjson::SizeType> ElementPathHolder;

} // namespace JsonSchemaValidator

#include "ValidationContext.inl"
