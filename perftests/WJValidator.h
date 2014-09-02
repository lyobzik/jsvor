#pragma once

#include <string>
#include <memory>

#include <wjelement.h>

#include <Validator.h>
#include <JsonErrors.h>

using namespace TestsCommon;

class WJValidator : public Validator {
public:
	WJValidator(std::string const &schema);
	virtual ~WJValidator() {};

	virtual jsvor::ValidationResult Validate(std::string const &json);

	virtual void Load(std::string const &json);
	virtual jsvor::ValidationResult Validate();

private:
	typedef std::shared_ptr<WJReaderPublic> WJReaderPtr;
	typedef std::shared_ptr<WJElementPublic> WJElementPtr;

	void Load(std::string const &json,
	          WJReaderPtr &document_reader, WJElementPtr &document, WJElementPtr &value);
	jsvor::ValidationResult Validate(WJElementPtr const &document, WJElementPtr const &value);

	static bool Parse(std::string const &json, WJReaderPtr &reader, WJElementPtr &root);

	WJReaderPtr schema_reader_;
	WJElementPtr schema_;

	WJReaderPtr document_reader_;
	WJElementPtr document_;
	WJElementPtr value_;
}; // class WJValidator : public Validator
