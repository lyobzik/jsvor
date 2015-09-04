#include "PrimitiveTypes.h"

#include <JsonSchema.h>
#include <JsonResolver.h>

#include "../Regex.h"
#include "../ValidationContext.h"

namespace JsonSchemaValidator {

JsonString::JsonString(JsonValue const &schema, JsonResolverPtr const &resolver,
                       std::string const &path)
	: JsonTypeImpl(schema, resolver, path)
	, min_length_(std::numeric_limits<size_t>::lowest())
	, max_length_(std::numeric_limits<size_t>::max())
	, pattern_() {

	GetChildValue(schema, "minLength", min_length_);
	GetChildValue(schema, "maxLength", max_length_);

	char const *pattern = nullptr;
	if (GetChildValue(schema, "pattern", pattern)) {
		pattern_ = Regex::Create(pattern);
	}
}

bool JsonString::CheckValueType(JsonValue const &json) const {
	return json.IsString();
}

void JsonString::CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const {
	if (json.GetStringLength() < min_length_) {
		return RaiseError<MinimalLengthError>(context, min_length_);
	}
	if (json.GetStringLength() > max_length_) {
		return RaiseError<MaximalLengthError>(context, max_length_);
	}

	if (pattern_ && !pattern_->IsCorrespond(GetValue<char const *>(json))) {
		return RaiseError<PatternError>(context, pattern_->Pattern());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Type>
JsonBaseNumber<Type>::JsonBaseNumber(JsonValue const &schema, JsonResolverPtr const &resolver,
                                     std::string const &path)
	: Parent(schema, resolver, path)
	, minimum_(std::numeric_limits<typename Parent::ValueType>::lowest())
	, maximum_(std::numeric_limits<typename Parent::ValueType>::max())
	, exclusive_minimum_(false)
	, exclusive_maximum_(false)
	, divisible_by_() {

	GetChildValue(schema, "minimum", minimum_);
	GetChildValue(schema, "maximum", maximum_);

	GetChildValue(schema, "exclusiveMinimum", exclusive_minimum_);
	GetChildValue(schema, "exclusiveMaximum", exclusive_maximum_);

	GetChildValue(schema, "divisibleBy", divisible_by_);
}

template <>
bool JsonBaseNumber<long long>::IsEqual(long long left, long long right) {
	return left == right;
}

template <>
bool JsonBaseNumber<double>::IsEqual(double left, double right) {
	return std::fabs(left - right) <= std::numeric_limits<double>::epsilon();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonNumber::JsonNumber(JsonValue const &schema, JsonResolverPtr const &resolver,
                       std::string const &path)
	: JsonBaseNumber(schema, resolver, path) {
}

bool JsonNumber::CheckValueType(JsonValue const &json) const {
	return json.IsNumber();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonInteger::JsonInteger(JsonValue const &schema, JsonResolverPtr const &resolver,
                         std::string const &path)
	: JsonBaseNumber(schema, resolver, path) {
}

bool JsonInteger::CheckValueType(JsonValue const &json) const {
	return json.IsInt64() || json.IsUint64() || json.IsInt() || json.IsUint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonBoolean::JsonBoolean(JsonValue const &schema, JsonResolverPtr const &resolver,
                         std::string const &path)
	: JsonTypeImpl(schema, resolver, path) {
}

bool JsonBoolean::CheckValueType(JsonValue const &json) const {
	return json.IsBool();
}

void JsonBoolean::CheckTypeRestrictions(JsonValue const &/*json*/,
	                                    ValidationContext &/*context*/) const {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonNull::JsonNull(JsonValue const &schema, JsonResolverPtr const &resolver,
                   std::string const &path)
	: JsonTypeImpl(schema, resolver, path) {
}

bool JsonNull::CheckValueType(JsonValue const &json) const {
	return json.IsNull();
}

void JsonNull::CheckTypeRestrictions(JsonValue const &/*json*/,
                                     ValidationContext &/*context*/) const {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonObject::JsonObject(JsonValue const &schema, JsonResolverPtr const &resolver,
                       std::string const &path)
	: JsonTypeImpl(schema, resolver, path)
	, properties_()
	, pattern_properties_()
	, may_contains_additional_properties_()
	, additional_properties_()
	, simple_dependencies_()
	, schema_dependencies_() {

	for (auto const &member : GetMembers(schema, "properties")) {
		properties_.insert({ GetValue<char const *>(member.name),
		                     CreateMember(member, resolver) });
	}

	for (auto const &member : GetMembers(schema, "patternProperties")) {
		pattern_properties_.push_back({ Regex::Create(GetValue<char const *>(member.name)),
		                                CreateMember(member, resolver) });
	}

	GetChildValue(schema, "additionalProperties", may_contains_additional_properties_);
	if (!may_contains_additional_properties_.exists) {
		GetChildValue(schema, "additionalProperties", additional_properties_, resolver, path);
	}

	for (auto const &member : GetMembers(schema, "dependencies")) {
		if (member.value.IsObject()) {
			schema_dependencies_.insert({ GetValue<char const *>(member.name),
			                              CreateMember(member, resolver) });
		}
		else if (member.value.IsArray()) {
			std::set<char const *, StrLess> strings;
			for (rapidjson::SizeType j = 0; j < member.value.Size(); ++j) {
				auto insert_result = strings.insert(GetValue<char const *>(member.value[j]));
				if (!insert_result.second) {
					throw IncorrectSchema(SchemaErrors::IncorrectDependencies);
				}
			}
			simple_dependencies_.insert({ GetValue<char const *>(member.name), strings });
		}
		else if (member.value.IsString()) {
			simple_dependencies_.insert({ GetValue<char const *>(member.name),
			                            { GetValue<char const *>(member.value) } });
		}
		else {
			throw IncorrectSchema(SchemaErrors::IncorrectDependencies);
		}
	}
}

bool JsonObject::CheckValueType(JsonValue const &json) const {
	return json.IsObject();
}

void JsonObject::CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const {
	for (auto const &member : GetMembers(json)) {
		bool described_property = false;
		char const *name = GetValue<char const *>(member.name);
		MemberPathHolder path_holder(name, context);
		auto propertyIt = properties_.find(name);
		if (propertyIt != properties_.end()) {
			propertyIt->second->Validate(member.value, context);
			if (!context.GetResult()) return;
			described_property = true;
		}
		for (auto const &pattern_property : pattern_properties_) {
			if (pattern_property.first->IsCorrespond(name)) {
				pattern_property.second->Validate(member.value, context);
				if (!context.GetResult()) return;
				described_property = true;
			}
		}

		if (!described_property) {
			if (may_contains_additional_properties_.exists) {
				if (!may_contains_additional_properties_.value) {
					path_holder.Reset();
					return RaiseError<AdditionalPropertyError>(context, name);
				}
			}
			else if (additional_properties_.exists) {
				additional_properties_.value->Validate(member.value, context);
				if (!context.GetResult()) return;
			}
		}

		if (!simple_dependencies_.empty()) {
			auto simpleArrayDependIt = simple_dependencies_.find(name);
			if (simpleArrayDependIt != simple_dependencies_.end()) {
				for (auto const &depend : simpleArrayDependIt->second) {
					if (!json.HasMember(depend)) {
						return RaiseError<DependenciesRestrictionsError>(context, name);
					}
				}
			}
		}
		if (!schema_dependencies_.empty()) {
			auto schemaDependIt = schema_dependencies_.find(name);
			if (schemaDependIt != schema_dependencies_.end()) {
				schemaDependIt->second->Validate(json, context);
				if (!context.GetResult()) return;
			}
		}
		path_holder.Reset();
	}
	for (auto const &property : properties_) {
		if (property.second->IsRequired() && !json.HasMember(property.first)) {
			return RaiseError<RequiredPropertyError>(context, property.first);
		}
	}
}

JsonTypePtr JsonObject::CreateMember(JsonValueMember const &member,
                                     JsonResolverPtr const &resolver) const {
	return JsonType::Create(member.value, resolver,
	                        MemberPath(GetValue<char const *>(member.name)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonArray::JsonArray(JsonValue const &schema, JsonResolverPtr const &resolver,
                     std::string const &path)
	: JsonTypeImpl(schema, resolver, path)
	, min_items_(0)
	, max_items_(std::numeric_limits<size_t>::max())
	, unique_items_(false)
	, items_()
	, items_array_()
	, may_contains_additional_items_()
	, additional_items_() {

	GetChildValue(schema, "minItems", min_items_);
	GetChildValue(schema, "maxItems", max_items_);

	GetChildValue(schema, "uniqueItems", unique_items_);

	GetChildValue(schema, "items", items_, resolver, path);
	GetChildValue(schema, "items", items_array_, resolver, path);

	GetChildValue(schema, "additionalItems", may_contains_additional_items_);
	if (!may_contains_additional_items_.exists) {
		GetChildValue(schema, "additionalItems", additional_items_, resolver, path);
	}
}

bool JsonArray::CheckValueType(JsonValue const &json) const {
	return json.IsArray();
}

void JsonArray::CheckTypeRestrictions(JsonValue const &json, ValidationContext &context) const {
	if (json.Size() < min_items_) {
		return RaiseError<MinimalItemsCountError>(context, min_items_);
	}
	if (json.Size() > max_items_) {
		return RaiseError<MaximalItemsCountError>(context, max_items_);
	}

	if (unique_items_) {
		for (rapidjson::SizeType i = 0; i < json.Size() - 1; ++i) {
			for (rapidjson::SizeType j = i + 1; j < json.Size(); ++j) {
				if (IsEqual(json[i], json[j])) {
					return RaiseError<UniqueItemsError>(context);
				}
			}
		}
	}

	if (items_.exists) {
		for (rapidjson::SizeType i = 0; i < json.Size(); ++i) {
			ElementPathHolder path_holder(i, context);
			items_.value->Validate(json[i], context);
			if (!context.GetResult()) return;
			path_holder.Reset();
		}
	}
	else if (items_array_.exists) {
		rapidjson::SizeType i = 0;
		for (; i < json.Size() && i < items_array_.value.size(); ++i) {
			ElementPathHolder path_holder(i, context);
			items_array_.value[i]->Validate(json[i], context);
			if (!context.GetResult()) return;
			path_holder.Reset();
		}
		if (i < json.Size()) {
			if (may_contains_additional_items_.exists) {
				if (!may_contains_additional_items_.value) {
					return RaiseError<AdditionalItemsError>(context);
				}
			}
			else if (additional_items_.exists) {
				for (; i < json.Size(); ++i) {
					ElementPathHolder path_holder(i, context);
					additional_items_.value->Validate(json[i], context);
					if (!context.GetResult()) return;
					path_holder.Reset();
				}
			}
		}
	}
}

} // namespace JsonSchemaValidator
