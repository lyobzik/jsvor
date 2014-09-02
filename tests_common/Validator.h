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
