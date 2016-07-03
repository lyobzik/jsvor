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

#include <set>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <regex>

#include "JsonTypeImpl.h"

namespace JsonSchemaValidator {

class JsonString : public JsonTypeImpl<char const *> {
public:
	JsonString(JsonValue const &schema, JsonResolverPtr const &resolver,
	           std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;

	size_t min_length_;
	size_t max_length_;

	RegexPtr pattern_;
	// Property format is ignored.
}; // class JsonString : public JsonTypeImpl<char const *>

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Type>
class JsonBaseNumber : public JsonTypeImpl<Type> {
	typedef JsonTypeImpl<Type> Parent;

public:
	JsonBaseNumber(JsonValue const &schema, JsonResolverPtr const &resolver,
	               std::string const &path);

private:
	typedef double DividerType;

	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;

	static bool IsEqual(Type left, Type right);
	static bool CheckDivisibility(Type value, DividerType divider);

	typename Parent::ValueType minimum_;
	typename Parent::ValueType maximum_;

	bool exclusive_minimum_;
	bool exclusive_maximum_;

	JsonTypeProperty<double> divisible_by_;
}; // class JsonBaseNumber : public JsonTypeImpl<Type>

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonNumber : public JsonBaseNumber<double> {
public:
	JsonNumber(JsonValue const &schema, JsonResolverPtr const &resolver,
	           std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
}; // class JsonNumber : public JsonBaseNumber<double>

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonInteger : public JsonBaseNumber<long long> {
public:
	JsonInteger(JsonValue const &schema, JsonResolverPtr const &resolver,
	            std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
}; // class JsonInteger : public JsonBaseNumber<long long>

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonBoolean : public JsonTypeImpl<bool> {
public:
	JsonBoolean(JsonValue const &schema, JsonResolverPtr const &resolver,
	            std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;
}; // class JsonBoolean : public JsonTypeImpl<bool>

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonNull : public JsonTypeImpl<JsonNullValue> {
public:
	JsonNull(JsonValue const &schema, JsonResolverPtr const &resolver,
	         std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;
}; // class JsonNull : public JsonTypeImpl<JsonNullValue>

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonObject : public JsonTypeImpl<JsonObjectValue> {
public:
	JsonObject(JsonValue const &schema, JsonResolverPtr const &resolver,
	           std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;

	JsonTypePtr CreateMember(JsonValueMember const &member, JsonResolverPtr const &resolver) const;

	std::map<char const *, JsonTypePtr, StrLess> properties_;

	std::vector<std::pair<RegexPtr, JsonTypePtr>> pattern_properties_;

	JsonTypeProperty<bool> may_contains_additional_properties_;
	JsonTypeProperty<JsonTypePtr> additional_properties_;

	std::map<char const *, std::set<char const *, StrLess>, StrLess> simple_dependencies_;
	std::map<char const *, JsonTypePtr, StrLess> schema_dependencies_;
}; // class JsonObject : public JsonTypeImpl<JsonObjectValue>

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonArray : public JsonTypeImpl<JsonArrayValue> {
public:
	JsonArray(JsonValue const &schema, JsonResolverPtr const &resolver,
	          std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;

	size_t min_items_;
	size_t max_items_;

	bool unique_items_;

	JsonTypeProperty<JsonTypePtr> items_;
	JsonTypeProperty<std::vector<JsonTypePtr>> items_array_;

	JsonTypeProperty<bool> may_contains_additional_items_;
	JsonTypeProperty<JsonTypePtr> additional_items_;
}; // class JsonArray : public JsonTypeImpl<JsonArrayValue>

} // namespace JsonSchemaValidator

#include "PrimitiveTypes.inl"
