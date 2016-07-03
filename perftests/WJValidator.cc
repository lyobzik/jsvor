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

#include "WJValidator.h"

WJValidator::WJValidator(std::string const &schema)
	: schema_reader_()
	, schema_()
	, document_reader_()
	, document_()
	, value_() {
	Parse(schema, schema_reader_, schema_);
}

jsvor::ValidationResult WJValidator::Validate(std::string const &json) {
	WJReaderPtr document_reader;
	WJElementPtr document, value;

	Load(json, document_reader, document, value);
	return Validate(document, value);
}

void WJValidator::Load(std::string const &json) {
	// Destroy objects in correct order.
	value_.reset();
	document_.reset();
	document_reader_.reset();

	Load(json, document_reader_, document_, value_);
}

jsvor::ValidationResult WJValidator::Validate() {
	return Validate(document_, value_);
}

void WJValidator::Load(std::string const &json,
	WJReaderPtr &document_reader, WJElementPtr &document, WJElementPtr &value) {
	Parse(json, document_reader, document);
	value.reset(WJEChild(document.get(), "data", WJE_GET), WJECloseDocument);
}

jsvor::ValidationResult WJValidator::Validate(WJElementPtr const &document,
                                              WJElementPtr const &value) {
	if (schema_) {
		WJElementPtr json = value ? value : document;
		WJESchemaValidate(schema_.get(), json.get(), nullptr, nullptr, nullptr, nullptr);
	}
	return jsvor::ValidationResult();
}

bool WJValidator::Parse(std::string const &json, WJReaderPtr &reader, WJElementPtr &root) {
	reader.reset(WJROpenMemDocument(const_cast<char *>(json.c_str()), nullptr, 0),
	             WJRCloseDocument);
	if (reader) {
		root.reset(WJEOpenDocument(reader.get(), nullptr, nullptr, nullptr),
		           WJECloseDocument);
		return static_cast<bool>(root);
	}
	return false;
}
