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

#include <vector>

#include <JsonErrors.h>

#include "Defs.h"
#include "RapidJsonHelpers.h"
#include "ValidationContext.h"

namespace JsonSchemaValidator {

template <typename PropertyType>
struct JsonTypeProperty {
	JsonTypeProperty()
		: exists(false)
		, value() {
	}

	bool exists;
	PropertyType value;
}; // class JsonTypeProperty

template <typename ValueType, typename ...Args>
bool GetChildValue(JsonValue const &json, char const *child_name,
                   JsonTypeProperty<ValueType> &value, Args... args) {
	value.exists = GetChildValue(json, child_name, value.value, args...);
	return value.exists;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonType {
public:
	JsonType(JsonValue const &schema, JsonResolverPtr const &resolver, std::string const &path);
	virtual ~JsonType() { }

	virtual void Validate(JsonValue const &json, ValidationContext &context) const;

	bool IsRequired() const;

	static JsonTypePtr Create(JsonValue const &value, JsonResolverPtr const &resolver,
	                          std::string const &path);

protected:
	std::string MemberPath(char const *member) const;

	template <typename DocumentErrorType, typename... Args>
	void RaiseError(ValidationContext &context, Args&&... args) const {
		auto &result = context.GetResult();
		result.SetError<DocumentErrorType>(args...);
	}
	static void RaiseError(SchemaErrors error);
	static JsonTypePtr CreateJsonTypeFromArrayElement(JsonValue const &schema,
	                                                  JsonValue const &type,
	                                                  JsonResolverPtr const &resolver,
	                                                  std::string const &path);

	static JsonTypeCreator GetCreator(JsonValue const &type);

private:
	void ValidateRef(JsonValue const &json, ValidationContext &context) const;
	void ValidateExtends(JsonValue const &json, ValidationContext &context) const;

	virtual bool CheckValueType(JsonValue const &json) const = 0;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const = 0;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationContext &context) const = 0;

	bool required_;
	std::string id_;
	std::string path_;
	JsonResolverPtr resolver_;
	std::vector<JsonTypePtr> extends_;
	JsonTypeProperty<std::string> ref_;
	// Properties schema, title, description, default not used for validation json-documents.
}; // class JsonType

///////////////////////////////////////////////////////////////////////////////////////////////////
bool GetValue(JsonValue const &json, JsonTypePtr &value, JsonResolverPtr const &resolver,
              std::string const &path);

bool GetValue(JsonValue const &json, std::vector<JsonTypePtr> &value,
              JsonResolverPtr const &resolver, std::string const &path);

} // namespace JsonSchemaValidator
