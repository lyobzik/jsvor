#include "../include/JsonErrors.h"

#include "RapidJsonHelpers.h"

namespace JsonSchemaValidator {

DocumentError::DocumentError(std::string const &path)
	: path_(path) {
}

DocumentError::~DocumentError() {
}

EnumValueError::EnumValueError(std::string const &path)
	: DocumentError(path) {
}

EnumValueError::~EnumValueError() {
}

std::string EnumValueError::GetDescription() const {
	return ToString("Attribute ", path_, " doesn't satisfy restriction on enumeration "
	                "of all possible values");
}

MinimalLengthError::MinimalLengthError(std::string const &path, size_t limit)
	: DocumentError(path)
	, limit_(limit) {
}

std::string MinimalLengthError::GetDescription() const {
	return ToString("Value length of attribute ", path_, " must be >= ", limit_, ".");
}

MaximalLengthError::MaximalLengthError(std::string const &path, size_t limit)
	: DocumentError(path)
	, limit_(limit) {
}

std::string MaximalLengthError::GetDescription() const {
	return ToString("Value length of attribute ", path_, " must be <= ", limit_, ".");
}

PatternError::PatternError(std::string const &path, std::string const &pattern)
	: DocumentError(path)
	, pattern_(pattern) {
}

std::string PatternError::GetDescription() const {
	return ToString("Attribute ", path_, " must pattern '", pattern_, "'.");
}

DivisibleValueError::DivisibleValueError(std::string const &path, double divisible_by)
	: DocumentError(path)
	, divisible_by_(divisible_by) {
}

std::string DivisibleValueError::GetDescription() const {
	return ToString("Attribute ", path_, " must be divisible by ", divisible_by_, ".");
}

AdditionalPropertyError::AdditionalPropertyError(std::string const &path)
	: DocumentError(path) {
}

std::string AdditionalPropertyError::GetDescription() const {
	return ToString("Attribute ", path_, " cannot contain additional property.");
}

DependenciesRestrictionsError::DependenciesRestrictionsError(std::string const &path,
                                                             std::string const &name)
	: DocumentError(path)
	, name_(name){
}

std::string DependenciesRestrictionsError::GetDescription() const {
	return ToString("Attribute ", path_, " doesn't satisfy dependency restriction of "
	                "property ", name_, ".");
}

RequiredPropertyError::RequiredPropertyError(std::string const &path, std::string const &property)
	: DocumentError(path)
	, property_(property){
}

std::string RequiredPropertyError::GetDescription() const {
	return ToString("Attribute ", path_, " must contain property '", property_, "'.");
}

MinimalItemsCountError::MinimalItemsCountError(std::string const &path, size_t limit)
	: DocumentError(path)
	, limit_(limit) {
}

std::string MinimalItemsCountError::GetDescription() const {
	return ToString("Array ", path_, " must contain >= ", limit_, " items.");
}

MaximalItemsCountError::MaximalItemsCountError(std::string const &path, size_t limit)
	: DocumentError(path)
	, limit_(limit) {
}

std::string MaximalItemsCountError::GetDescription() const {
	return ToString("Array ", path_, " must contain <= '", limit_, " items.");
}

UniqueItemsError::UniqueItemsError(std::string const &path)
	: DocumentError(path) {
}

std::string UniqueItemsError::GetDescription() const {
	return ToString("Array ", path_, " cannot contain equal items.");
}

AdditionalItemsError::AdditionalItemsError(std::string const &path)
	: DocumentError(path) {
}

std::string AdditionalItemsError::GetDescription() const {
	return ToString("Array ", path_, " cannot contain additional items.");
}

DisallowTypeError::DisallowTypeError(std::string const &path)
	: DocumentError(path) {
}

std::string DisallowTypeError::GetDescription() const {
	return ToString("Attribute ", path_, " satisfies one of disallowed types.");
}

NeitherTypeError::NeitherTypeError(std::string const &path)
	: DocumentError(path) {
}

std::string NeitherTypeError::GetDescription() const {
	return ToString("Attribute ", path_, " doesn't satisfy any allow type.");
}

TypeError::TypeError(std::string const &path)
	: DocumentError(path) {
}

std::string TypeError::GetDescription() const {
	return ToString("Attribute ", path_, " has incorrect type.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ValidationResult::ValidationResult()
	: error_() {
}

std::string ValidationResult::ErrorDescription() const {
	return error_ ? error_->GetDescription() : std::string();
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
