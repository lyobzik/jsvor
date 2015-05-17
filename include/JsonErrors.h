#pragma once

#include <string>
#include <exception>

#include "../include/JsonDefs.h"
#include "../lib/RapidJsonHelpers.h"

namespace JsonSchemaValidator {

// Errors that may occur in json-document validation.
class DocumentError {
public:
	DocumentError(std::string const &path);
	virtual ~DocumentError();

	virtual std::string GetDescription() const = 0;

protected:
	std::string path_;
}; // class DocumentError

class EnumValueError : public DocumentError {
public:
	EnumValueError(std::string const &path);
	virtual ~EnumValueError();

	virtual std::string GetDescription() const;
}; // class EnumValueError : public DocumentError

class MinimalLengthError : public DocumentError {
public:
	MinimalLengthError(std::string const &path, size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MinimalLengthError : public DocumentError

class MaximalLengthError : public DocumentError {
public:
	MaximalLengthError(std::string const &path, size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MaximalLengthError : public DocumentError

class PatternError : public DocumentError {
public:
	PatternError(std::string const &path, std::string const &pattern);

	virtual std::string GetDescription() const;

private:
	std::string pattern_;
}; // class PatternError : public DocumentError

template<typename Type>
class MinimumValueError : public DocumentError {
public:
	MinimumValueError(std::string const &path, Type const &limit, bool exclusive)
		: DocumentError(path)
		, limit_(limit)
		, exclusive_(exclusive) {
	}

	virtual std::string GetDescription() const {
		return ToString("Attribute ", path_, " must be ", exclusive_ ? ">" : ">=", limit_, ".");
	}

private:
	Type limit_;
	bool exclusive_;
}; // class MinimumValueError : public DocumentError

template<typename Type>
class MaximumValueError : public DocumentError {
public:
	MaximumValueError(std::string const &path, Type const &limit, bool exclusive)
		: DocumentError(path)
		, limit_(limit)
		, exclusive_(exclusive) {
	}

	virtual std::string GetDescription() const {
		return ToString("Attribute ", path_, " must be ", exclusive_ ? "<" : "<=", limit_, ".");
	}

private:
	Type limit_;
	bool exclusive_;
}; // class MaximumValueError : public DocumentError

class DivisibleValueError : public DocumentError {
public:
	DivisibleValueError(std::string const &path, double divisible_by);

	virtual std::string GetDescription() const;

private:
	double divisible_by_;
}; // class DivisibleValueError : public DocumentError

class AdditionalPropertyError : public DocumentError {
public:
	AdditionalPropertyError(std::string const &path);

	virtual std::string GetDescription() const;
}; // class AdditionalPropertyError : public DocumentError

class DependenciesRestrictionsError : public DocumentError {
public:
	DependenciesRestrictionsError(std::string const &path, std::string const &name);

	virtual std::string GetDescription() const;

private:
	std::string name_;
}; // class DependenciesRestrictionsError : public DocumentError

class RequiredPropertyError : public DocumentError {
public:
	RequiredPropertyError(std::string const &path, std::string const &property);

	virtual std::string GetDescription() const;

private:
	std::string property_;
}; // class RequiredPropertyError : public DocumentError

class MinimalItemsCountError : public DocumentError {
public:
	MinimalItemsCountError(std::string const &path, size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MinimalItemsCountError : public DocumentError

class MaximalItemsCountError : public DocumentError {
public:
	MaximalItemsCountError(std::string const &path, size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MaximalItemsCountError : public DocumentError

class UniqueItemsError : public DocumentError {
public:
	UniqueItemsError(std::string const &path);

	virtual std::string GetDescription() const;
}; // class UniqueItemsError : public DocumentError

class AdditionalItemsError : public DocumentError {
public:
	AdditionalItemsError(std::string const &path);

	virtual std::string GetDescription() const;
}; // class AdditionalItemsError : public DocumentError

class DisallowTypeError : public DocumentError {
public:
	DisallowTypeError(std::string const &path);

	virtual std::string GetDescription() const;
}; // class DisallowTypeError : public DocumentError

class NeitherTypeError : public DocumentError {
public:
	NeitherTypeError(std::string const &path);

	virtual std::string GetDescription() const;
}; // class NeitherTypeError : public DocumentError

class TypeError : public DocumentError {
public:
	TypeError(std::string const &path);

	virtual std::string GetDescription() const;
}; // class TypeError : public DocumentError

typedef std::unique_ptr<DocumentError> DocumentErrorPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
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

	template<typename DocumentErrorType, typename... Args>
	void SetError(Args&&... args) {
		error_.reset(new DocumentErrorType(std::forward<Args>(args)...));
	}

	std::string ErrorDescription() const;

	operator bool() const;

private:
	DocumentErrorPtr error_;
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
	explicit IncorrectDocument(ValidationResult &&result);
	IncorrectDocument(IncorrectDocument &&incorrect_document) = default;

	virtual ~IncorrectDocument() throw() { }

	virtual char const *what() const throw();

	ValidationResult const& GetResult() const;

private:
	IncorrectDocument(IncorrectDocument const &) = delete;
	IncorrectDocument& operator=(IncorrectDocument const &) = delete;

	ValidationResult result_;
	mutable std::string error_description_;
}; // class IncorrectDocument : public Error

} // namespace JsonSchemaValidator
