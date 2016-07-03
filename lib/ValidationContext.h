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
