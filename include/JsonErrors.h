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

#include <list>
#include <string>
#include <exception>

#include "../include/JsonDefs.h"
#include "../lib/RapidJsonHelpers.h"

namespace JsonSchemaValidator {

// Errors that may occur in json-document validation.
class DocumentError {
public:
	DocumentError();
	virtual ~DocumentError();

	virtual std::string GetDescription() const = 0;

	std::string GetPath() const;
	void AddPath(std::string const &path);

protected:
	std::list<std::string> path_parts_;
}; // class DocumentError

class EnumValueError : public DocumentError {
public:
	EnumValueError();
	virtual ~EnumValueError();

	virtual std::string GetDescription() const;
}; // class EnumValueError : public DocumentError

class MinimalLengthError : public DocumentError {
public:
	explicit MinimalLengthError(size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MinimalLengthError : public DocumentError

class MaximalLengthError : public DocumentError {
public:
	explicit MaximalLengthError(size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MaximalLengthError : public DocumentError

class PatternError : public DocumentError {
public:
	explicit PatternError(std::string const &pattern);

	virtual std::string GetDescription() const;

private:
	std::string pattern_;
}; // class PatternError : public DocumentError

template<typename Type>
class MinimumValueError : public DocumentError {
public:
	MinimumValueError(Type const &limit, bool exclusive)
		: DocumentError()
		, limit_(limit)
		, exclusive_(exclusive) {
	}

	virtual std::string GetDescription() const {
		return ToString("Attribute ", GetPath(), " must be ", exclusive_ ? ">" : ">=", limit_, ".");
	}

private:
	Type limit_;
	bool exclusive_;
}; // class MinimumValueError : public DocumentError

template<typename Type>
class MaximumValueError : public DocumentError {
public:
	MaximumValueError(Type const &limit, bool exclusive)
		: DocumentError()
		, limit_(limit)
		, exclusive_(exclusive) {
	}

	virtual std::string GetDescription() const {
		return ToString("Attribute ", GetPath(), " must be ", exclusive_ ? "<" : "<=", limit_, ".");
	}

private:
	Type limit_;
	bool exclusive_;
}; // class MaximumValueError : public DocumentError

class DivisibleValueError : public DocumentError {
public:
	explicit DivisibleValueError(double divisible_by);

	virtual std::string GetDescription() const;

private:
	double divisible_by_;
}; // class DivisibleValueError : public DocumentError

class AdditionalPropertyError : public DocumentError {
public:
	AdditionalPropertyError(std::string const &name);

	virtual std::string GetDescription() const;

private:
	std::string name_;
}; // class AdditionalPropertyError : public DocumentError

class DependenciesRestrictionsError : public DocumentError {
public:
	explicit DependenciesRestrictionsError(std::string const &name);

	virtual std::string GetDescription() const;

private:
	std::string name_;
}; // class DependenciesRestrictionsError : public DocumentError

class RequiredPropertyError : public DocumentError {
public:
	explicit RequiredPropertyError(std::string const &property);

	virtual std::string GetDescription() const;

private:
	std::string property_;
}; // class RequiredPropertyError : public DocumentError

class MinimalItemsCountError : public DocumentError {
public:
	explicit MinimalItemsCountError(size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MinimalItemsCountError : public DocumentError

class MaximalItemsCountError : public DocumentError {
public:
	explicit MaximalItemsCountError(size_t limit);

	virtual std::string GetDescription() const;

private:
	size_t limit_;
}; // class MaximalItemsCountError : public DocumentError

class UniqueItemsError : public DocumentError {
public:
	UniqueItemsError();

	virtual std::string GetDescription() const;
}; // class UniqueItemsError : public DocumentError

class AdditionalItemsError : public DocumentError {
public:
	AdditionalItemsError();

	virtual std::string GetDescription() const;
}; // class AdditionalItemsError : public DocumentError

class DisallowTypeError : public DocumentError {
public:
	DisallowTypeError();

	virtual std::string GetDescription() const;
}; // class DisallowTypeError : public DocumentError

class NeitherTypeError : public DocumentError {
public:
	NeitherTypeError();

	virtual std::string GetDescription() const;
}; // class NeitherTypeError : public DocumentError

class TypeError : public DocumentError {
public:
	TypeError();

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
	void AddPath(std::string const &path);

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
