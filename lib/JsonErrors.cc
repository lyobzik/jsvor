#include "../include/JsonErrors.h"

#include "RapidJsonHelpers.h"

namespace JsonSchemaValidator {

ValidationResult::ValidationResult()
	: error_(DocumentErrors::None)
	, requirements_()
	, json_()
	, detailed_() {
}

void ValidationResult::SetError(DocumentErrors error, std::string const &requirements,
                                JsonValue const &json, std::string const &path) {
	error_ = error;
	requirements_ = requirements;
	json_ = ToString(json);
	path_ = path;
}

DocumentErrors ValidationResult::Error() const {
	return error_;
}

char const *ValidationResult::ErrorDescription() const {
	switch (error_) {
	case DocumentErrors::None:
		return "None";

	case DocumentErrors::EnumValue:
		return "Inspected document not satisfied restriction on value enum.";

	case DocumentErrors::MinimalLength:
		return "Inspected document not satisfied restriction on minimal length.";
	case DocumentErrors::MaximalLength:
		return "Inspected document not satisfied restriction on maximal length.";
	case DocumentErrors::Pattern:
		return "Inspected document not satisfied restriction on pattern.";

	case DocumentErrors::MinimumValue:
		return "Inspected document not satisfied restriction on minimum.";
	case DocumentErrors::MaximumValue:
		return "Inspected document not satisfied restriction on maximum.";
	case DocumentErrors::DivisibleValue:
		return "Inspected document not satisfied restriction on divisible.";

	case DocumentErrors::AdditionalProperty:
		return "Inspected document not satisfied restriction on additional property.";
	case DocumentErrors::DependenciesRestrictions:
		return "Inspected document not satisfied restriction on dependencies.";
	case DocumentErrors::RequiredProperty:
		return "Inspected document not satisfied restriction on required property.";

	case DocumentErrors::MinimalItemsCount:
		return "Inspected document not satisfied restriction on minimal items count.";
	case DocumentErrors::MaximalItemsCount:
		return "Inspected document not satisfied restriction on maximal items count.";
	case DocumentErrors::UniqueItems:
		return "Inspected document not satisfied restriction on unique items.";
	case DocumentErrors::AdditionalItems:
		return "Inspected document not satisfied restriction on additional items.";

	case DocumentErrors::DisallowType:
		return "Inspected document not satisfied disallow type.";
	case DocumentErrors::NeitherType:
		return "Inspected document not satisfied neither type.";
	case DocumentErrors::Type:
		return "Inspected document not satisfied restriction on type.";

	case DocumentErrors::Unknown:
		return "Unknown error";
	}
	return "Unknown error";
}

char const *ValidationResult::RequirementsDescription() const {
	return requirements_.c_str();
}

char const *ValidationResult::Json() const {
	return json_.c_str();
}

char const *ValidationResult::DetailedErrorDescription() const {
	detailed_ = ToString(ErrorDescription(), " ", RequirementsDescription(), " ", Json(), " at ", path_);
	return detailed_.c_str();
}

ValidationResult::operator bool() const {
	return error_ == DocumentErrors::None;
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
IncorrectDocument::IncorrectDocument(ValidationResult const &result)
: Error()
, result_(result) {
}

char const *IncorrectDocument::what() const throw() {
	return result_.DetailedErrorDescription();
}

ValidationResult const& IncorrectDocument::GetResult() const {
	return result_;
}

} // namespace JsonSchemaValidator
