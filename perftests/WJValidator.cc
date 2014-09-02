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
