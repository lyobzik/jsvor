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
	JsonCustomType(JsonValue const &schema, JsonResolverPtr const &resolver,
	               std::string const &path);

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationContext &context) const;

	JsonTypePtr custom_type_;
}; // class JsonCustomType : public JsonType

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonAny : public JsonType
{
public:
	JsonAny(JsonValue const &schema, JsonResolverPtr const &resolver,
	        std::string const &path);

	virtual void Validate(JsonValue const &json, ValidationContext &context) const;

private:
	friend class JsonUnionType;

	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationContext &context) const;

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
	JsonUnionType(JsonValue const &schema, JsonResolverPtr const &resolver,
	              std::string const &path);

	virtual void Validate(JsonValue const &json,ValidationContext &context) const;

private:
	virtual bool CheckValueType(JsonValue const &json) const;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationContext &context) const;

	std::set<JsonTypePtr> type_;
}; // class JsonUnionType : public JsonType

} // namespace JsonSchemaValidator
