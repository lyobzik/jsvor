#pragma once

#include <set>
#include <string>
#include <vector>
#include <utility>
#include <regex>

#include "../Defs.h"
#include "../RapidJsonHelpers.h"

#include "JsonTypeImpl.h"

namespace JsonSchemaValidator {

class JsonCustomType : public JsonType {
public:
	JsonCustomType(JsonValue const &schema, JsonResolverPtr const &resolver);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationResult &result) const;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationResult &result) const;

	JsonTypePtr custom_type_;
}; // class JsonCustomType : public JsonType

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonAny : public JsonType
{
public:
	JsonAny(JsonValue const &schema, JsonResolverPtr const &resolver);

	virtual void Validate(JsonValue const &json, ValidationResult &result) const;

private:
	friend class JsonUnionType;

	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationResult &result) const;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationResult &result) const;

	JsonTypePtr const &GetSchema(JsonValue const &json) const;

	std::set<JsonTypePtr> disallow_;

	JsonTypePtr string_;
	JsonTypePtr number_;
	JsonTypePtr integer_;
	JsonTypePtr boolean_;
	JsonTypePtr object_;
	JsonTypePtr array_;
	JsonTypePtr null_;
}; // class JsonAny : public JsonType

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonUnionType : public JsonType
{
public:
	JsonUnionType(JsonValue const &schema, JsonResolverPtr const &resolver);

	virtual void Validate(JsonValue const &json, ValidationResult &result) const;

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationResult &result) const;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationResult &result) const;

	std::set<JsonTypePtr> type_;
}; // class JsonUnionType : public JsonType

} // namespace JsonSchemaValidator
