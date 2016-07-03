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

#include "../include/JsonErrors.h"

#include "RapidJsonHelpers.h"

namespace JsonSchemaValidator {

DocumentError::DocumentError()
	: path_parts_() {
}

DocumentError::~DocumentError() {
}

std::string DocumentError::GetPath() const {
	std::string result_path;
	for (auto it = path_parts_.begin(); it != path_parts_.end(); ++it) {
		result_path.append("/");
		result_path.append(*it);
	}

	return result_path.empty() ? "/" : result_path;
}

void DocumentError::AddPath(std::string const &path) {
	path_parts_.push_front(path);
}

EnumValueError::EnumValueError()
	: DocumentError() {
}

EnumValueError::~EnumValueError() {
}

std::string EnumValueError::GetDescription() const {
	return ToString("Element ", GetPath(), " doesn't satisfy restriction on enumeration "
	                "of all possible values");
}

MinimalLengthError::MinimalLengthError(size_t limit)
	: DocumentError()
	, limit_(limit) {
}

std::string MinimalLengthError::GetDescription() const {
	return ToString("Value length of element ", GetPath(), " must be >= ", limit_, ".");
}

MaximalLengthError::MaximalLengthError(size_t limit)
	: DocumentError()
	, limit_(limit) {
}

std::string MaximalLengthError::GetDescription() const {
	return ToString("Value length of element ", GetPath(), " must be <= ", limit_, ".");
}

PatternError::PatternError(std::string const &pattern)
	: DocumentError()
	, pattern_(pattern) {
}

std::string PatternError::GetDescription() const {
	return ToString("Element ", GetPath(), " must match pattern '", pattern_, "'.");
}

DivisibleValueError::DivisibleValueError(double divisible_by)
	: DocumentError()
	, divisible_by_(divisible_by) {
}

std::string DivisibleValueError::GetDescription() const {
	return ToString("Element ", GetPath(), " must be divisible by ", divisible_by_, ".");
}

AdditionalPropertyError::AdditionalPropertyError(std::string const &name)
	: DocumentError()
	, name_(name) {
}

std::string AdditionalPropertyError::GetDescription() const {
	return ToString("Element ", GetPath(), " cannot contain additional property '", name_, "'.");
}

DependenciesRestrictionsError::DependenciesRestrictionsError(std::string const &name)
	: DocumentError()
	, name_(name){
}

std::string DependenciesRestrictionsError::GetDescription() const {
	return ToString("Element ", GetPath(), " doesn't satisfy dependency restriction of "
	                "property ", name_, ".");
}

RequiredPropertyError::RequiredPropertyError(std::string const &property)
	: DocumentError()
	, property_(property){
}

std::string RequiredPropertyError::GetDescription() const {
	return ToString("Element ", GetPath(), " must contain property '", property_, "'.");
}

MinimalItemsCountError::MinimalItemsCountError(size_t limit)
	: DocumentError()
	, limit_(limit) {
}

std::string MinimalItemsCountError::GetDescription() const {
	return ToString("Array ", GetPath(), " must contain >= ", limit_, " items.");
}

MaximalItemsCountError::MaximalItemsCountError(size_t limit)
	: DocumentError()
	, limit_(limit) {
}

std::string MaximalItemsCountError::GetDescription() const {
	return ToString("Array ", GetPath(), " must contain <= '", limit_, " items.");
}

UniqueItemsError::UniqueItemsError()
	: DocumentError() {
}

std::string UniqueItemsError::GetDescription() const {
	return ToString("Array ", GetPath(), " cannot contain equal items.");
}

AdditionalItemsError::AdditionalItemsError()
	: DocumentError() {
}

std::string AdditionalItemsError::GetDescription() const {
	return ToString("Array ", GetPath(), " cannot contain additional items.");
}

DisallowTypeError::DisallowTypeError()
	: DocumentError() {
}

std::string DisallowTypeError::GetDescription() const {
	return ToString("Element ", GetPath(), " satisfies one of disallowed types.");
}

NeitherTypeError::NeitherTypeError()
	: DocumentError() {
}

std::string NeitherTypeError::GetDescription() const {
	return ToString("Element ", GetPath(), " doesn't satisfy any allow type.");
}

TypeError::TypeError()
	: DocumentError() {
}

std::string TypeError::GetDescription() const {
	return ToString("Element ", GetPath(), " has incorrect type.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ValidationResult::ValidationResult()
	: error_() {
}

std::string ValidationResult::ErrorDescription() const {
	return error_ ? error_->GetDescription() : std::string();
}

void ValidationResult::AddPath(std::string const &path) {
	if (error_) {
		error_->AddPath(path);
	}
}

ValidationResult::operator bool() const {
	return !error_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
char const *IncorrectSchema::what() const throw() {
	switch (error_) {
	case SchemaErrors::None:
		return "None";

	case SchemaErrors::IncorrectCustomType:
		return "Incorrect custom type of schema.";
	case SchemaErrors::IncorrectDisallowType:
		return "Incorrect disallow type of schema.";
	case SchemaErrors::IncorrectUnionType:
		return "Incorrect union type of schema.";

	case SchemaErrors::NotJsonObject:
		return "Schema must be a json-object.";
	case SchemaErrors::CantDetectType:
		return "Cannot detect type of schema.";
	case SchemaErrors::IncorrectType:
		return "Incorrect type of schema.";

	case SchemaErrors::IncorrectMember:
		return "Incorrect member value.";
	case SchemaErrors::IncorrectDependencies:
		return "Incorrect dependencies valu.e";

	case SchemaErrors::Unknown:
		return "Unknown error.";
	}
	return "Unknown error.";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
IncorrectDocument::IncorrectDocument(ValidationResult &&result)
	: Error()
	, result_(std::move(result))
	, error_description_() {
}

char const *IncorrectDocument::what() const throw() {
	error_description_ = result_.ErrorDescription();
	return error_description_.c_str();
}

ValidationResult const& IncorrectDocument::GetResult() const {
	return result_;
}

} // namespace JsonSchemaValidator
