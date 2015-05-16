#pragma once

#include <vector>

#include <JsonErrors.h>

#include "Defs.h"
#include "RapidJsonHelpers.h"

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

	virtual void Validate(JsonValue const &json, ValidationResult &result) const;

	bool IsRequired() const;

	static JsonTypePtr Create(JsonValue const &value, JsonResolverPtr const &resolver,
	                          std::string const &path);

protected:
	std::string MemberPath(char const *member) const;

	void RaiseError(DocumentErrors error, std::string const &requirements,
	                       JsonValue const &json, ValidationResult &result) const; //TODO
	static void RaiseError(SchemaErrors error);
	static JsonTypePtr CreateJsonTypeFromArrayElement(JsonValue const &schema,
	                                                  JsonResolverPtr const &resolver,
	                                                  std::string const &path);

	static JsonTypeCreator GetCreator(JsonValue const &type);

private:
	void ValidateRef(JsonValue const &json, ValidationResult &result) const;
	void ValidateExtends(JsonValue const &json, ValidationResult &result) const;

	virtual bool CheckValueType(JsonValue const &json) const = 0;
	virtual void CheckTypeRestrictions(JsonValue const &json, ValidationResult &result) const = 0;
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationResult &result) const = 0;

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
