#pragma once

#include <set>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <regex>

#include "Defs.h"
#include "JsonType.h"
#include "RapidJsonHelpers.h"

namespace JsonSchemaValidator {

template <typename PropertyType>
struct JsonTypeProperty {
	JsonTypeProperty()
		: exists(false)
		, value() {
	}

	bool Check() const {
		return false;
	}

	bool exists;
	PropertyType value;
}; // class JsonTypeProperty

template <typename ValueType>
bool GetChildValue(JsonValue const &json, char const *child_name, JsonTypeProperty<ValueType> &value) {
	value.exists = GetChildValue(json, child_name, value.value);
	return value.exists;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Type>
class JsonTypeImpl : public JsonType
{
public:
	JsonTypeImpl(JsonValue const &schema_part, JsonResolverPtr const &resolver);

	virtual bool IsRequired() const;

protected:
	typedef Type ValueType;

private:
	std::string JsonPath() const;

	virtual void ValidateRef(JsonValue const &inspected_value, ValidationResult &result) const;
	virtual void ValidateExtends(JsonValue const &inspected_value, ValidationResult &result) const;
	virtual void CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const;
	virtual void CheckEnumsRestrictions(JsonValue const &inspected_value, ValidationResult &result) const;

	JsonTypeProperty<std::vector<Type>> enum_;
	bool required_;

	std::vector<JsonTypePtr> extends_;

	std::string id_;
	JsonTypeProperty<std::string> ref_;
	std::string schema_;

	std::string json_path_;

	JsonResolverPtr resolver_;

	// Properties title, description, default not used for validation json-documents.
}; // class JsonTypeImpl

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Класс для описания ограничений, налагаемых на строки. Проверка на соответствие формату
// (свойство format) не выполняется.
class JsonString : public JsonTypeImpl<std::string> {
public:
	JsonString(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const;

	size_t min_length_;
	size_t max_length_;

	RegexPtr pattern_;
}; // class JsonString : public JsonTypeImpl<std::string>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Type>
class JsonBaseNumber : public JsonTypeImpl<Type> {
    typedef JsonTypeImpl<Type> Parent;

public:
	JsonBaseNumber(JsonValue const &schema_part, JsonResolverPtr const &resolver)
        : Parent(schema_part, resolver)
        , minimum_(std::numeric_limits<typename Parent::ValueType>::min())
        , maximum_(std::numeric_limits<typename Parent::ValueType>::max())
		, exclusive_minimum_(false)
		, exclusive_maximum_(false)
		, divisible_by_() {
		GetChildValue(schema_part, "minimum", minimum_);
		GetChildValue(schema_part, "maximum", maximum_);

		GetChildValue(schema_part, "exclusiveMinimum", exclusive_minimum_);
		GetChildValue(schema_part, "exclusiveMaximum", exclusive_maximum_);

		GetChildValue(schema_part, "divisibleBy", divisible_by_);
	}

protected:
    typename Parent::ValueType minimum_;
    typename Parent::ValueType maximum_;

	bool exclusive_minimum_;
	bool exclusive_maximum_;

	JsonTypeProperty<double> divisible_by_;
}; // class JsonBaseNumber : public JsonTypeImpl<Type>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class JsonNumber : public JsonBaseNumber<double> {
public:
	JsonNumber(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const;
}; // class JsonNumber : public JsonBaseNumber<double>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class JsonInteger : public JsonBaseNumber<long long> {
public:
	JsonInteger(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const;
}; // class JsonInteger : public JsonBaseNumber<long long>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class JsonBoolean : public JsonTypeImpl<bool> {
public:
	JsonBoolean(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const;
}; // class JsonBoolean : public JsonTypeImpl<bool>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class JsonObject : public JsonTypeImpl<JsonObjectValue>
{
public:
	JsonObject(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const;

	std::map<String, JsonTypePtr> properties_;

	std::vector<std::pair<RegexPtr, JsonTypePtr>> pattern_properties_;

	JsonTypeProperty<bool> may_contains_additional_properties_;
	JsonTypeProperty<JsonTypePtr> additional_properties_;

	std::map<String, std::string> simple_dependencies_;
	std::map<String, std::set<std::string>> simple_array_dependencies_;
	std::map<String, JsonTypePtr> schema_dependencies_;
}; // class JsonObject : public JsonTypeImpl

class JsonArray : public JsonTypeImpl<JsonArrayValue>
{
public:
	JsonArray(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const;

	size_t min_items_;
	size_t max_items_;

	bool unique_items_;

	JsonTypeProperty<JsonTypePtr> items_;
	JsonTypeProperty<std::vector<JsonTypePtr>> items_array_;

	JsonTypeProperty<bool> may_contains_additional_items_;
	JsonTypeProperty<JsonTypePtr> additional_items_;
}; // class JsonArray : public JsonTypeImpl

class JsonNull : public JsonTypeImpl<JsonNullValue>
{
public:
	JsonNull(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const;
}; // class JsonNull : public JsonTypeImpl

class JsonAny : public JsonType
{
public:
	JsonAny(JsonValue const &schema_part, JsonResolverPtr const &resolver);

	virtual void Validate(JsonValue const &inspected_value) const;
	virtual void Validate(JsonValue const &inspected_value, ValidationResult &result) const;
	virtual bool IsRequired() const;

private:
	friend class JsonUnionType;

	virtual void ValidateRef(JsonValue const &inspected_value) const;
	virtual void ValidateExtends(JsonValue const &inspected_value) const;
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &value) const;
	virtual void CheckEnumsRestrictions(JsonValue const &value) const;

	virtual void ValidateRef(JsonValue const &inspected_value, ValidationResult &result) const;
	virtual void ValidateExtends(JsonValue const &inspected_value, ValidationResult &result) const;
	virtual bool CheckValueType(JsonValue const &value, ValidationResult &result) const;
	virtual void CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const;
	virtual void CheckEnumsRestrictions(JsonValue const &value, ValidationResult &result) const;

	JsonTypePtr const &GetSchema(JsonValue const &inspected_document) const;

	bool required_;
	std::string id_;

	std::set<JsonTypePtr> disallow_;

	JsonTypePtr string_;
	JsonTypePtr number_;
	JsonTypePtr integer_;
	JsonTypePtr boolean_;
	JsonTypePtr object_;
	JsonTypePtr array_;
	JsonTypePtr null_;
}; // class JsonAny : public JsonTypeImpl

class JsonCustomType : public JsonTypeImpl<int>
{
public:
	JsonCustomType(JsonValue const &schema_part, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &value) const;
	virtual void CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const;

	JsonTypePtr custom_type_;
}; // class JsonCustomType : public JsonTypeImpl

class JsonUnionType : public JsonAny
{
public:
	JsonUnionType(JsonValue const &schema, JsonResolverPtr const &resolver);

	virtual void Validate(JsonValue const &inspected_value) const;
	virtual void Validate(JsonValue const &inspected_value, ValidationResult &result) const;

private:
	std::set<JsonTypePtr> type_;
}; // class JsonUnionType : public JsonAny

} // namespace JsonSchemaValidator
