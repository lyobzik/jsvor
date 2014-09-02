#pragma once

#include "Defs.h"

namespace JsonSchemaValidator {

// Класс для сокрытия реализации работы с регулярными выражениями.
class Regex {
public:
	Regex();
	virtual ~Regex();

	virtual bool IsCorrespond(char const *value) const = 0;

	static RegexPtr Create(char const *pattern);
}; // class Regex

} // namespace JsonSchemaValidator
