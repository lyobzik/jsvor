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
