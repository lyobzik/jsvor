#pragma once

#include <string>
#include <exception>

#include "../include/JsonDefs.h"

namespace JsonSchemaValidator {

// Errors that may occur in json-document validation.
enum class DocumentErrors {
	None,

	EnumValue,

	MinimalLength,
	MaximalLength,
	Pattern,

	MinimumValue,
	MaximumValue,
	DivisibleValue,

	AdditionalProperty,
	DependenciesRestrictions,
	RequiredProperty,

	MinimalItemsCount,
	MaximalItemsCount,
	UniqueItems,
	AdditionalItems,

	DisallowType,
	NeitherType,
	Type,

	Unknown
}; // enum class DocumentErrors

// Errors that may occur in parsing of json-schema.
enum class SchemaErrors {
	None,

	IncorrectCustomType,
	IncorrectDisallowType,
	IncorrectUnionType,

	NotJsonObject,
	CantDetectType,
	IncorrectType,

	IncorrectMember,
	IncorrectDependencies,

	Unknown
}; // enum class SchemaErrors

///////////////////////////////////////////////////////////////////////////////////////////////////
// Provides result of document validation with full description of error causes.
class ValidationResult {
public:
	ValidationResult();

	void SetError(DocumentErrors error, std::string const &description, std::string const &path);

	DocumentErrors Error() const;
	char const *ErrorDescription() const;
	char const *Description() const;

	char const *DetailedErrorDescription() const;

	operator bool() const;

private:
	DocumentErrors error_;
	std::string description_;
	std::string path_;
	std::string mutable detailed_;
}; // class ValidationResult

///////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for all errors.
class Error : public std::exception {
}; // class Error : public std::exception

// Error of parsing json.
class IncorrectJson : public Error {
public:
	explicit IncorrectJson(std::string const &error)
		: Error()
		, error_(error) {
	}

	virtual ~IncorrectJson() throw() { }

	virtual char const *what() const throw() {
		return error_.c_str();
	}

private:
	std::string error_;
}; // class IncorrectJson : public Error {

// Error of creating JsonSchema.
class IncorrectSchema : public Error {
public:
	explicit IncorrectSchema(SchemaErrors error)
		: Error()
		, error_(error) {
	}

	virtual ~IncorrectSchema() throw() { }

	virtual char const *what() const throw();

private:
	SchemaErrors error_;
}; // class IncorrectSchema : public Error

// Error of validating json-document.
class IncorrectDocument : public Error {
public:
	explicit IncorrectDocument(ValidationResult const &result);

	virtual ~IncorrectDocument() throw() { }

	virtual char const *what() const throw();

	ValidationResult const& GetResult() const;

private:
	ValidationResult result_;
}; // class IncorrectDocument : public Error

} // namespace JsonSchemaValidator
