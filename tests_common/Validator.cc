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
