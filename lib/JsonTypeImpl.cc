#include "JsonTypeImpl.h"

#include <JsonSchema.h>
#include <JsonResolver.h>

#include "Regex.h"

namespace JsonSchemaValidator
{


template <>
bool GetValue<JsonTypePtr>(JsonValue const &json, JsonTypePtr &value)
{
    if (json.IsObject()) {
        value = JsonType::Create(json, JsonResolverPtr());
        return true;
    }
    return false;
}

template <>
bool GetValue<std::vector<JsonTypePtr>>(JsonValue const &json, std::vector<JsonTypePtr> &value)
{
    if (json.IsArray()) {
        for (JsonSizeType i = 0; i < json.Size(); ++i) {
            value.push_back(JsonType::Create(json[i], JsonResolverPtr()));
        }
        return true;
    }
    return false;
}

template <typename ValueType, typename DividerType>
bool CheckDivisibility(ValueType const &value, DividerType const &divider) {
	double division = static_cast<double>(value) / static_cast<double>(divider);
	double round_division = static_cast<double>(static_cast<long long>(division + 0.5));
	return std::fabs(division - round_division) > std::numeric_limits<ValueType>::epsilon();
}


struct MemberRange {
	JsonValueConstMemberIterator begin_;
	JsonValueConstMemberIterator end_;
};

JsonValueConstMemberIterator begin(MemberRange const &range) {
	return range.begin_;
}

JsonValueConstMemberIterator end(MemberRange const &range) {
	return range.end_;
}

MemberRange GetMembers(JsonValue const &value, char const *child_name) {
	rapidjson::Value::Member const *children = value.FindMember(child_name);
	if (children) {
		if (children->value.IsObject()) {
			return MemberRange{ children->value.MemberBegin(), children->value.MemberEnd() };
		}
		else {
			throw JsonSchema::IncorrectSchema(std::string("Incorrect value of ") + child_name + std::string("."));
		}
	}
	return MemberRange{ nullptr, nullptr };
}

MemberRange GetMembers(JsonValue const &value) {
	if (value.IsObject()) {
		return MemberRange{ value.MemberBegin(), value.MemberEnd() };
	}
	return MemberRange{ nullptr, nullptr };
}


template <>
bool JsonTypeProperty<bool>::Check() const {
	return !exists || !value;
}

//template <>
//bool JsonTypeProperty<JsonTypePtr>::Check() const {
//	if (exists) {
//		return value->Validate(it->value, result);
//	}
//	return true;
//}

} // namespace
namespace JsonSchemaValidator {



template <typename Type>
struct JsonTypeTraits
{
	static bool ContainsValue(std::vector<Type> const &values, Type const &value)
	{
		return std::binary_search(values.begin(), values.end(), value);
	}
}; // class JsonTypeTraits

struct JsonContainsChecker
{
	template <typename Type>
	static bool ContainsValue(std::vector<Type> const &values, Type const &value)
	{
		auto const it = std::find(begin(values), end(values), value);
		return it != end(values);
	}
};

template <>
struct JsonTypeTraits<JsonObjectValue> : public JsonContainsChecker
{
}; // class JsonTypeTraits

template <>
struct JsonTypeTraits<JsonArrayValue> : public JsonContainsChecker
{
}; // class JsonTypeTraits

template <>
struct JsonTypeTraits<JsonNullValue> : public JsonContainsChecker
{
}; // class JsonTypeTraits
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Type>
JsonTypeImpl<Type>::JsonTypeImpl(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: required_(false)
	, enum_()
	, id_()
	, ref_()
	, resolver_(resolver)
{
	GetChildValue(schema_part, "enum", enum_);

	GetChildValue(schema_part, "required", required_);

	if (!GetChildValue(schema_part, "extends", extends_)) {
		JsonTypePtr extend_json_type;
		if (GetChildValue(schema_part, "extends", extend_json_type)) {
			extends_.push_back(extend_json_type);
		}
	}

	//GetChildValue(schema_part, "id", id_);	
	GetChildValue(schema_part, "ref", ref_);
	//std::string schema_;
}

template <typename Type>
bool JsonTypeImpl<Type>::IsRequired() const
{
	return required_;
}

template <typename Type>
std::string JsonTypeImpl<Type>::JsonPath() const {
	return json_path_;
}

template <typename Type>
void JsonTypeImpl<Type>::ValidateRef(JsonValue const &inspected_value, ValidationResult &result) const {
	if (ref_.exists && resolver_) {
		JsonSchemaPtr ref_schema = resolver_->Resolve(ref_.value);
		if (ref_schema) {
			ref_schema->Validate(inspected_value, result);
		}
	}
}

template <typename Type>
void JsonTypeImpl<Type>::ValidateExtends(JsonValue const &inspected_value, ValidationResult &result) const
{
	if (!extends_.empty()) {
		for (auto const &json_type : extends_) {
			json_type->Validate(inspected_value, result);
		}
	}
}

template <typename Type>
void JsonTypeImpl<Type>::CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const
{
}

template <typename Type>
void JsonTypeImpl<Type>::CheckEnumsRestrictions(JsonValue const &inspected_value, ValidationResult &result) const
{
	if (enum_.exists) {
		Type value = GetValue<Type>(inspected_value);
		if (!JsonTypeTraits<Type>::ContainsValue(enum_.value, value)) {
			return RaiseError(DocumentErrors::EnumValue, result);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonString::JsonString(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonTypeImpl(schema_part, resolver)
	, pattern_()
	, min_length_(std::numeric_limits<size_t>::min())
	, max_length_(std::numeric_limits<size_t>::max()) {
	GetChildValue(schema_part, "minLength", min_length_);
	GetChildValue(schema_part, "maxLength", max_length_);

	char const *pattern(nullptr);
	if (GetChildValue(schema_part, "pattern", pattern)) {
		pattern_ = Regex::Create(pattern);
	}
	// Даже если присутствует поле format игнорируем его.
}

bool JsonString::CheckValueType(JsonValue const &inspected_value) const {
	return inspected_value.IsString();
}

void JsonString::CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const {
	if (inspected_value.GetStringLength() < min_length_) {
		return RaiseError(DocumentErrors::MinimalLength, result);
	}
	if (inspected_value.GetStringLength() > max_length_) {
		return RaiseError(DocumentErrors::MaximalLength, result);
	}

	if (pattern_ && !pattern_->IsCorrespond(GetValue<char const *>(inspected_value))) {
		return RaiseError(DocumentErrors::Pattern, result);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonNumber::JsonNumber(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonBaseNumber(schema_part, resolver) {
}

bool JsonNumber::CheckValueType(JsonValue const &inspected_value) const {
	return inspected_value.IsNumber();
}

void JsonNumber::CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const {
	ValueType value = GetValue<ValueType>(inspected_value);
	if ((value < minimum_) ||
		(exclusive_minimum_ && std::fabs(value - minimum_) < std::numeric_limits<ValueType>::epsilon())) {
		return RaiseError(DocumentErrors::MinimumValue, result);
	}
	if ((maximum_ < value) ||
		(exclusive_maximum_ && std::fabs(maximum_ - value) < std::numeric_limits<ValueType>::epsilon())) {
		return RaiseError(DocumentErrors::MaximumValue, result);
	}

	if (divisible_by_.exists && CheckDivisibility(value, divisible_by_.value)) {
		return RaiseError(DocumentErrors::DivisibleValue, result);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonInteger::JsonInteger(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonBaseNumber(schema_part, resolver) {
}

bool JsonInteger::CheckValueType(JsonValue const &inspected_value) const {
	return inspected_value.IsInt64() || inspected_value.IsUint64() ||
		inspected_value.IsInt() || inspected_value.IsUint();
}

void JsonInteger::CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const {
	ValueType value = GetValue<ValueType>(inspected_value);
	if ((value < minimum_) || (exclusive_minimum_ && value == minimum_)) {
		return RaiseError(DocumentErrors::MinimumValue, result);
	}
	if ((maximum_ < value) || (exclusive_maximum_ && maximum_ == value)) {
		return RaiseError(DocumentErrors::MaximumValue, result);
	}

	if (divisible_by_.exists && CheckDivisibility(value, divisible_by_.value)) {
		return RaiseError(DocumentErrors::DivisibleValue, result);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonBoolean::JsonBoolean(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonTypeImpl(schema_part, resolver) {
}

bool JsonBoolean::CheckValueType(JsonValue const &inspected_value) const {
	return inspected_value.IsBool();
}

void JsonBoolean::CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonObject::JsonObject(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonTypeImpl(schema_part, resolver)
	, properties_()
	, pattern_properties_()
	, may_contains_additional_properties_()
	, additional_properties_()
	, simple_dependencies_()
	, simple_array_dependencies_()
	, schema_dependencies_()
{
	for (auto const &member : GetMembers(schema_part, "properties")) {
		properties_.insert({ String::Copy(member.name), JsonType::Create(member.value, resolver) });
	}

	for (auto const &member : GetMembers(schema_part, "patternProperties")) {
		RegexPtr pattern = Regex::Create(GetValue<char const *>(member.name));
		pattern_properties_.push_back({ pattern, JsonType::Create(member.value, resolver) });
	}

	GetChildValue(schema_part, "additionalProperties", may_contains_additional_properties_);
	if (!may_contains_additional_properties_.exists) {
		GetChildValue(schema_part, "additionalProperties", additional_properties_);
	}

	for (auto const &member : GetMembers(schema_part, "dependencies")) {
		if (member.value.IsString()) {
			simple_dependencies_.insert({ String::Copy(member.name), GetValue<std::string>(member.value) });
		}
		else if (member.value.IsObject()) {
			schema_dependencies_.insert({ String::Copy(member.name), JsonType::Create(member.value, resolver) });
		}
		else if (member.value.IsArray()) {
			std::set<std::string> strings;
			for (rapidjson::SizeType j = 0; j < member.value.Size(); ++j) {
				auto insert_result = strings.insert(GetValue<std::string>(member.value[j]));
				if (!insert_result.second) {
					throw JsonSchema::IncorrectSchema("Incorrect value of dependencies.");
				}
			}
			simple_array_dependencies_.insert({ String::Copy(member.name), strings });
		}
		else {
			throw JsonSchema::IncorrectSchema("Incorrect value of dependencies.");
		}
	}
}

bool JsonObject::CheckValueType(JsonValue const &inspected_value) const
{
	return inspected_value.IsObject();
}

void JsonObject::CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const
{
	for (auto const &member : GetMembers(value)) {
		bool described_property = false;
		String name(String::Wrap(member.name));
		auto propertyIt = properties_.find(name);
		if (propertyIt != properties_.end()) {
			propertyIt->second->Validate(member.value, result);
			if (!result) return;
			described_property = true;
		}
		for (auto const &pattern_property : pattern_properties_) {
			if (pattern_property.first->IsCorrespond(name.c_str())) {
				pattern_property.second->Validate(member.value, result);
				if (!result) return;
				described_property = true;
			}
		}

		if (!described_property) {
			//if (use_make_contains_ && !may_contains_additional_properties_) {
			if (may_contains_additional_properties_.exists && 
				!may_contains_additional_properties_.value) {
				return RaiseError(DocumentErrors::AdditionalProperty, result);
			}
			else if (additional_properties_.exists) {
				additional_properties_.value->Validate(member.value, result);
				if (!result) return;
			}
		}

		if (!simple_dependencies_.empty()) {
			auto simpleDependIt = simple_dependencies_.find(name);
			if (simpleDependIt != simple_dependencies_.end()) {
				if (!value.HasMember(simpleDependIt->second.c_str())) {
					return RaiseError(DocumentErrors::DependenciesRestrictions, result);
				}
			}
		}
		if (!simple_array_dependencies_.empty()) {
			auto simpleArrayDependIt = simple_array_dependencies_.find(name);
			if (simpleArrayDependIt != simple_array_dependencies_.end()) {
				for (auto const &depend : simpleArrayDependIt->second) {
					if (!value.HasMember(depend.c_str())) {
						return RaiseError(DocumentErrors::DependenciesRestrictions, result);
					}
				}
			}
		}
		if (!schema_dependencies_.empty()) {
			auto schemaDependIt = schema_dependencies_.find(name);
			if (schemaDependIt != schema_dependencies_.end()) {
				schemaDependIt->second->Validate(value, result);
				if (!result) return;
			}
		}
	}
	for (auto const &property : properties_) {
		if (property.second->IsRequired() && !value.HasMember(property.first.c_str())) {
			return RaiseError(DocumentErrors::RequiredProperty, result);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonArray::JsonArray(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonTypeImpl(schema_part, resolver)
	, min_items_(0)
	, max_items_(std::numeric_limits<size_t>::max())
	, unique_items_(false)
	, items_()
	, items_array_()
	, additional_items_()
	, may_contains_additional_items_()
{
	GetChildValue(schema_part, "minItems", min_items_);
	GetChildValue(schema_part, "maxItems", max_items_);

	GetChildValue(schema_part, "uniqueItems", unique_items_);

	GetChildValue(schema_part, "items", items_);
	GetChildValue(schema_part, "items", items_array_);

	GetChildValue(schema_part, "additionalItems", may_contains_additional_items_);
	if (!may_contains_additional_items_.exists) {
		GetChildValue(schema_part, "additionalItems", additional_items_);
	}
}

bool JsonArray::CheckValueType(JsonValue const &inspected_value) const
{
	return inspected_value.IsArray();
}

void JsonArray::CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const
{
	// Проверяем, что для массива выполняются ограничение на его размер.
	if (value.Size() < min_items_) {
		return RaiseError(DocumentErrors::MinimalItemsCount, result);
	}
	if (value.Size() > max_items_) {
		return RaiseError(DocumentErrors::MaximalItemsCount, result);
	}
	// Проверяем, что для массива выполняется условие уникальности элементов.
	if (unique_items_) {
		for (rapidjson::SizeType i = 0; i < value.Size() - 1; ++i) {
			for (rapidjson::SizeType j = i + 1; j < value.Size(); ++j) {
				if (IsEqual(value[i], value[j])) {
					return RaiseError(DocumentErrors::UniqueItems, result);
				}
			}
		}
	}
	// Проверяем, что элементы массива соответствуют требованиям.
	if (items_.exists) {
		for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
			items_.value->Validate(value[i], result);
			if (!result) return;
		}
	}
	else if (items_array_.exists) {
		rapidjson::SizeType i = 0;
		for (; i < value.Size() && i < items_array_.value.size(); ++i) {
			items_array_.value[i]->Validate(value[i], result);
			if (!result) return;
		}
		if (i < value.Size()) {
			if (may_contains_additional_items_.exists && !may_contains_additional_items_.value) {
				return RaiseError(DocumentErrors::AdditionalItems, result);
			}
			else if (additional_items_.exists) {
				for (; i < value.Size(); ++i) {
					additional_items_.value->Validate(value[i], result);
					if (!result) return;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonNull::JsonNull(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonTypeImpl(schema_part, resolver)
{
}

bool JsonNull::CheckValueType(JsonValue const &inspected_value) const
{
	return inspected_value.IsNull();
}

void JsonNull::CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonAny::JsonAny(JsonValue const &schema, JsonResolverPtr const &resolver)
	: JsonType()
	, required_(false)
	, id_()
	, disallow_()
	, string_(std::make_shared<JsonString>(schema, resolver))
	, number_(std::make_shared<JsonNumber>(schema, resolver))
	, integer_(std::make_shared<JsonInteger>(schema, resolver))
	, boolean_(std::make_shared<JsonBoolean>(schema, resolver))
	, object_(std::make_shared<JsonObject>(schema, resolver))
	, array_(std::make_shared<JsonArray>(schema, resolver))
	, null_(std::make_shared<JsonNull>(schema, resolver))
{
	GetChildValue(schema, "required", required_);
	JsonValueMember const *disallow = schema.FindMember("disallow");
	if (disallow) {
		if (disallow->value.IsArray()) {
			for (JsonSizeType i = 0; i < disallow->value.Size(); ++i) {
				JsonTypePtr json_type = CreateJsonTypeFromArrayElement(disallow->value[i], resolver);
				disallow_.insert(json_type);
			}
		}
		else if (disallow->value.IsObject() || disallow->value.IsString()) {
			JsonTypePtr json_type = CreateJsonTypeFromArrayElement(disallow->value, resolver);
			disallow_.insert(json_type);
		}
		else {
			RaiseSchemaError("Incorrect disallow type of schema");
		}
	}
	GetChildValue(schema, "id", id_);
}

void JsonAny::Validate(JsonValue const &inspected_value) const
{
	for (auto const &disallow : disallow_) {
		bool is_disallowed = false;
		try {
			disallow->Validate(inspected_value);
			is_disallowed = true;
		}
		catch (JsonSchema::IncorrectDocument const &/*error*/) {
			// Ошибки игнорируем, так как их наличие означает соответствие документа ограничениям данной схемы.
		}
		if (is_disallowed) {
			RaiseError(ValidationResult(DocumentErrors::DisallowType));
		}
	}
	GetSchema(inspected_value)->Validate(inspected_value);
}

void JsonAny::Validate(JsonValue const &inspected_value, ValidationResult &result) const
{
	for (auto const &disallow : disallow_) {
		ValidationResult disallow_result;
		disallow->Validate(inspected_value, disallow_result);
		if (disallow_result) {
			return RaiseError(DocumentErrors::DisallowType, result);
		}
	}
	GetSchema(inspected_value)->Validate(inspected_value, result);
}

bool JsonAny::IsRequired() const
{
	return required_;
}

void JsonAny::ValidateRef(JsonValue const &inspected_value) const
{
}

void JsonAny::ValidateExtends(JsonValue const &inspected_value) const
{
}

bool JsonAny::CheckValueType(JsonValue const &value) const
{
	return true;
}

void JsonAny::CheckTypeRestrictions(JsonValue const &inspected_value) const
{
}

void JsonAny::CheckEnumsRestrictions(JsonValue const &value) const
{
}

void JsonAny::ValidateRef(JsonValue const &inspected_value, ValidationResult &result) const
{
}

void JsonAny::ValidateExtends(JsonValue const &inspected_value, ValidationResult &result) const
{
}

bool JsonAny::CheckValueType(JsonValue const &value, ValidationResult &result) const
{
	return true;
}

void JsonAny::CheckTypeRestrictions(JsonValue const &inspected_value, ValidationResult &result) const
{
}

void JsonAny::CheckEnumsRestrictions(JsonValue const &value, ValidationResult &result) const
{
}


JsonTypePtr const &JsonAny::GetSchema(JsonValue const &inspected_document) const
{
	switch (inspected_document.GetType()) {
	case rapidjson::kFalseType:
	case rapidjson::kTrueType:
		return boolean_;
	case rapidjson::kObjectType:
		return object_;
	case rapidjson::kArrayType:
		return array_;
	case rapidjson::kStringType:
		return string_;
	case rapidjson::kNumberType:
		if (inspected_document.IsDouble()) {
			return number_;
		}
		return integer_;
	case rapidjson::kNullType:
		return null_;
	}
	return null_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonCustomType::JsonCustomType(JsonValue const &schema_part, JsonResolverPtr const &resolver)
	: JsonTypeImpl(schema_part, resolver)
	, custom_type_()
{
	rapidjson::Value::Member const *type = schema_part.FindMember("type");
	if (!type) {
		RaiseSchemaError("Incorrect custom type of schema");
	}
	custom_type_ = JsonType::Create(type->value, resolver);
}

bool JsonCustomType::CheckValueType(JsonValue const &value) const
{
	return true; //TODO: а правильно ли это?
}

void JsonCustomType::CheckTypeRestrictions(JsonValue const &value, ValidationResult &result) const
{
	return custom_type_->Validate(value, result);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

JsonUnionType::JsonUnionType(JsonValue const &schema, JsonResolverPtr const &resolver)
	: JsonAny(schema, resolver)
	, type_()
{
	JsonValueMember const *type = schema.FindMember("type");
	if (type) {
		if (type->value.IsArray()) {
			for (JsonSizeType i = 0; i < type->value.Size(); ++i) {
				JsonTypePtr json_type = CreateJsonTypeFromArrayElement(type->value[i], resolver);
				type_.insert(json_type);
			}
		}
		else if (type->value.IsObject()) {
			JsonTypeCreator creator = GetCreator(type->value);
			type_.insert(creator(schema, resolver));
		}
		else {
			RaiseSchemaError("Incorrect type of schema");
		}
	}
}

void JsonUnionType::Validate(JsonValue const &inspected_value) const
{
	JsonAny::CheckTypeRestrictions(inspected_value);

	for (auto const &type : type_) {
		try {
			type->Validate(inspected_value);
			return;
		}
		catch (JsonSchema::IncorrectDocument const &/*error*/) {
			// Здесь ошибки игнорируются, так как документ может удовлетворять требованиям другого типа.
		}
	}
	RaiseError(ValidationResult(DocumentErrors::NeitherType));
}

void JsonUnionType::Validate(JsonValue const &inspected_value, ValidationResult &result) const
{
	JsonAny::CheckTypeRestrictions(inspected_value);

	for (auto const &type : type_) {
		ValidationResult type_result;
		type->Validate(inspected_value, type_result);
		if (type_result) {
			return;
		}
		// Здесь ошибки игнорируются, так как документ может удовлетворять требованиям другого типа.
	}
	return RaiseError(DocumentErrors::NeitherType, result);
}


} // namespace JsonSchemaValidator
