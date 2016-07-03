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

#include "Validator.h"

namespace TestsCommon {

RJValidator::RJValidator(std::string const &schema)
	: schema_(schema)
	, document_()
	, value_(nullptr) {
}

jsvor::ValidationResult RJValidator::Validate(std::string const &json) {
	jsvor::JsonDocument document;
	jsvor::JsonValue const *value = nullptr;

	Load(json, document, value);
	return Validate(document, value);
}

void RJValidator::Load(std::string const &json) {
	Load(json, document_, value_);
}

jsvor::ValidationResult RJValidator::Validate() {
	return Validate(document_, value_);
}

void RJValidator::Load(std::string const &json,
                       jsvor::JsonDocument &document, jsvor::JsonValue const * &value) {
	document.Parse<0>(json.c_str());
	if (document.IsObject() && document.HasMember("data")) {
		value = &document["data"];
	}
}

jsvor::ValidationResult RJValidator::Validate(jsvor::JsonDocument const &document,
                                              jsvor::JsonValue const *value) {
	jsvor::ValidationResult result;
	if (value) {
		schema_.Validate(*value, result);
	}
	else {
		schema_.Validate(document, result);
	}
	return result;
}

} // namespace TestsCommon
