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
