#pragma once

#include <string>

#include "Defs.h"

namespace JsonSchemaValidator {

// Класс для сокрытия реализации работы с регулярными выражениями.
class Regex {
public:
	Regex(char const *pattern);
	virtual ~Regex();

	virtual bool IsCorrespond(char const *value) const = 0;

	char const *Pattern() const;

	static RegexPtr Create(char const *pattern);

private:
	std::string pattern_;
}; // class Regex

} // namespace JsonSchemaValidator
