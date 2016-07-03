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

#include <JsonSchema.h>
#include <JsonErrors.h>
#include <JsonDefs.h>

namespace TestsCommon {

namespace jsvor = JsonSchemaValidator;

class Validator {
public:
	Validator() = default;
	virtual ~Validator() {};

	virtual jsvor::ValidationResult Validate(std::string const &json) = 0;

	virtual void Load(std::string const &json) = 0;
	virtual jsvor::ValidationResult Validate() = 0;

private:
	Validator(Validator const&) = delete;
	Validator& operator=(Validator const&) = delete;
}; // class Validator

typedef std::shared_ptr<Validator> ValidatorPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
class RJValidator : public Validator {
public:
	RJValidator(std::string const &schema);
	virtual ~RJValidator() {};

	virtual jsvor::ValidationResult Validate(std::string const &json);

	virtual void Load(std::string const &json);
	virtual jsvor::ValidationResult Validate();

private:
	void Load(std::string const &json,
	          jsvor::JsonDocument &document, jsvor::JsonValue const * &value);
	jsvor::ValidationResult Validate(jsvor::JsonDocument const &document,
	                                 jsvor::JsonValue const *value);

	jsvor::JsonSchema schema_;

	jsvor::JsonDocument document_;
	jsvor::JsonValue const *value_;
}; // class RJValidator : public Validator

} // namespace TestsCommon
