#pragma once

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <limits>
#include <memory>

#include "RapidJsonDefs.h"

namespace JsonSchemaValidator {

struct MemberRange {
	JsonValueConstMemberIterator begin_;
	JsonValueConstMemberIterator end_;
};

JsonValueConstMemberIterator begin(MemberRange const &range);
JsonValueConstMemberIterator end(MemberRange const &range);

MemberRange GetMembers(JsonValue const &json, char const *child_name);
MemberRange GetMembers(JsonValue const &json);

JsonValueMember const *FindMember(JsonValue const &json, char const *child_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
class BasicJsonValue {
public:
	BasicJsonValue();
	explicit BasicJsonValue(JsonValue const &value);

	void SetValue(JsonValue const &value) {
		alive_value_ = &value;
	}
	JsonValue const &GetValue() const {
		return *alive_value_;
	}

private:
	JsonValue const *alive_value_;
}; // class BasicJsonValue


///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonObjectValue : private BasicJsonValue {
public:
	JsonObjectValue();
	explicit JsonObjectValue(JsonValue const &value);

	using BasicJsonValue::SetValue;
	using BasicJsonValue::GetValue;
}; // class JsonObjectValue : private BasicJsonValue

class JsonArrayValue : private BasicJsonValue {
public:
	JsonArrayValue();
	explicit JsonArrayValue(JsonValue const &value);

	using BasicJsonValue::SetValue;
	using BasicJsonValue::GetValue;
}; // class JsonArrayValue : private BasicJsonValue

///////////////////////////////////////////////////////////////////////////////////////////////////
class JsonNullValue : private BasicJsonValue {
public:
	JsonNullValue();
	explicit JsonNullValue(JsonValue const &value);

	using BasicJsonValue::SetValue;
	using BasicJsonValue::GetValue;
}; // class JsonNullValue : private BasicJsonValue

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename ValueType>
bool GetValue(JsonValue const &/*json*/, ValueType &/*value*/) {
	assert(!"Incorrect type for GetValue(JsonValue const &json, ValueType &value).");
	return false;
}

template <>
inline bool GetValue<bool>(JsonValue const &json, bool &value) {
	if (json.IsBool()) {
		value = json.GetBool();
		return true;
	}
	return false;
}

template <>
inline bool GetValue<long long>(JsonValue const &json, long long &value) {
	if (json.IsNumber()) {
		if (json.IsInt() || json.IsInt64()) {
			value = json.GetInt64();
			return true;
		}
		else if (json.IsUint() || json.IsUint64()) {
			value = json.GetUint64();
			return true;
		}
	}
	return false;
}

template <>
inline bool GetValue<size_t>(JsonValue const &json, size_t &value) {
	if (json.IsNumber()) {
		if (json.IsInt() || json.IsInt64()) {
			value = static_cast<size_t>(json.GetInt64());
			return true;
		}
		else if (json.IsUint() || json.IsUint64()) {
			value = static_cast<size_t>(json.GetUint64());
			return true;
		}
	}
	return false;
}

template <>
inline bool GetValue<double>(JsonValue const &json, double &value) {
	if (json.IsNumber()) {
		value = json.GetDouble();
		return true;
	}
	return false;
}

template <>
inline bool GetValue<char const *>(JsonValue const &json, char const *&value) {
	if (json.IsString()) {
		value = json.GetString();
		return true;
	}
	return false;
}

template <>
inline bool GetValue<std::string>(JsonValue const &json, std::string &value) {
	if (json.IsString()) {
		value = json.GetString();
		return true;
	}
	return false;
}

template <>
inline bool GetValue<JsonObjectValue>(JsonValue const &json, JsonObjectValue &value) {
	if (json.IsObject()) {
		value.SetValue(json);
		return true;
	}
	return false;
}

template <>
inline bool GetValue<JsonArrayValue>(JsonValue const &json, JsonArrayValue &value) {
	if (json.IsArray()) {
		value.SetValue(json);
		return true;
	}
	return false;
}

template <>
inline bool GetValue<JsonNullValue>(JsonValue const &json, JsonNullValue &value) {
	if (json.IsNull()) {
		value.SetValue(json);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Type>
Type GetValue(JsonValue const &/*json*/) {
	assert(!"Incorrect type for GetValue(JsonValue const &json).");
	return Type();
}

template <>
inline double GetValue<double>(JsonValue const &json) {
	return json.GetDouble();
}

template <>
inline long long GetValue<long long>(JsonValue const &json) {
	if (json.IsUint() || json.IsUint64()) {
		return json.GetUint64();
	}
	return json.GetInt64();
}

template <>
inline char const *GetValue<char const *>(JsonValue const &json) {
	return json.GetString();
}

template <>
inline std::string GetValue<std::string>(JsonValue const &json) {
	return json.GetString();
}

template <>
inline JsonObjectValue GetValue<JsonObjectValue>(JsonValue const &json) {
	return JsonObjectValue(json);
}

template <>
inline JsonArrayValue GetValue<JsonArrayValue>(JsonValue const &json) {
	return JsonArrayValue(json);
}

template <>
inline JsonNullValue GetValue<JsonNullValue>(JsonValue const &json) {
	return JsonNullValue(json);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename ValueType, typename ...Args>
bool GetChildValue(JsonValue const &json, char const *child_name, ValueType &value, Args... args) {
	if (!json.IsObject()) {
		return false;
	}
	JsonValueMember const *child = FindMember(json, child_name);
	if (child) {
		return GetValue(child->value, value, args...);
	}
	return false;
}

template <typename ValueType, typename ...Args>
bool GetChildValue(JsonValue const &json, char const *child_name, std::vector<ValueType> &value,
                   Args... args) {
	if (!json.IsObject()) {
		return false;
	}
	JsonValueMember const *child = FindMember(json, child_name);
	if (child && child->value.IsArray()) {
		for (JsonSizeType index = 0; index < child->value.Size(); ++index) {
			ValueType element_value;
			if (GetValue(child->value[index], element_value, args...)) {
				value.push_back(element_value);
			}
		}
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////s
struct StrLess {
	bool operator()(char const *lhs, char const *rhs) const {
		return strcmp(lhs, rhs) < 0;
	}
}; // struct StrLess

bool IsEqual(JsonValue const &left, JsonValue const &right);

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetLastError(JsonDocument const &json);

} // namespace JsonSchemaValidator

