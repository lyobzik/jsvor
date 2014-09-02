#include "RapidJsonHelpers.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <JsonSchema.h>

namespace JsonSchemaValidator {

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
			throw IncorrectSchema(SchemaErrors::IncorrectMember); // TODO: json_path + child_name
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

///////////////////////////////////////////////////////////////////////////////////////////////////
BasicJsonValue::BasicJsonValue()
	: alive_value_(nullptr) {
}

BasicJsonValue::BasicJsonValue(JsonValue const &value)
	: alive_value_(&value) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonObjectValue::JsonObjectValue()
	: BasicJsonValue() {
}

JsonObjectValue::JsonObjectValue(JsonValue const &value)
	: BasicJsonValue(value) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonArrayValue::JsonArrayValue()
	: BasicJsonValue() {
}

JsonArrayValue::JsonArrayValue(JsonValue const &value)
	: BasicJsonValue(value) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonNullValue::JsonNullValue()
	: BasicJsonValue() {
}

JsonNullValue::JsonNullValue(JsonValue const &value)
	: BasicJsonValue(value) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool IsEqual(JsonValue const &left, JsonValue const &right) {
	if (left.GetType() != right.GetType()) {
		return false;
	}

	using namespace rapidjson;
	if (left.IsObject()) {
		for (auto const &member : GetMembers(left)) {
			auto it = right.FindMember(GetValue<char const *>(member.name));
			if (!it || !IsEqual(member.value, it->value)) {
				return false;
			}
		}
		for (auto const &member : GetMembers(right)) {
			auto it = left.FindMember(GetValue<char const *>(member.name));
			if (!it) {
				return false;
			}
		}
		return true;
	}
	if (left.IsArray()) {
		if (left.Size() != right.Size()) {
			return false;
		}
		for (SizeType i = 0; i < left.Size(); ++i) {
			if (!IsEqual(left[i], right[i])) {
				return false;
			}
		}
		return true;
	}
	if (left.IsString()) {
		return strcmp(GetValue<char const *>(left), GetValue<char const *>(right)) == 0;
	}
	if (left.IsNumber()) {
		if (!left.IsDouble() && !right.IsDouble()) {
			return GetValue<long long>(left) == GetValue<long long>(right);
		}
		return std::fabs(GetValue<double>(left) -GetValue<double>(right)) <
			std::numeric_limits<double>::epsilon();
	}
	if (left.IsBool()) {
		return true;
	}
	return false;
}

} // namespace JsonSchemaValidator
