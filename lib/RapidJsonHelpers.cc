#include "RapidJsonHelpers.h"

#include <sstream>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/rapidjson.h>

#include <JsonSchema.h>

namespace JsonSchemaValidator {

JsonValueConstMemberIterator begin(MemberRange const &range) {
	return range.begin_;
}

JsonValueConstMemberIterator end(MemberRange const &range) {
	return range.end_;
}

MemberRange GetMembers(JsonValue const &value, char const *child_name) {
	JsonValueMember const *children = FindMember(value, child_name);
	if (children) {
		if (children->value.IsObject()) {
			return MemberRange{ children->value.MemberBegin(), children->value.MemberEnd() };
		}
		else {
			throw IncorrectSchema(SchemaErrors::IncorrectMember); // TODO: json_path + child_name
		}
	}
	return MemberRange{ JsonValue::MemberIterator(), JsonValue::MemberIterator() };
}

MemberRange GetMembers(JsonValue const &value) {
	if (value.IsObject()) {
		return MemberRange{ value.MemberBegin(), value.MemberEnd() };
	}
	return MemberRange{ JsonValue::MemberIterator(), JsonValue::MemberIterator() };
}

JsonValueMember const *FindMember(JsonValue const &json, char const *child_name) {
	auto const &it = json.FindMember(child_name);
	if (it != json.MemberEnd()) {
		return it.operator->();
	}
	return nullptr;
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
			JsonValueMember const *it = FindMember(right, GetValue<char const *>(member.name));
			if (!it || !IsEqual(member.value, it->value)) {
				return false;
			}
		}
		for (auto const &member : GetMembers(right)) {
			JsonValueMember const *it = FindMember(left, GetValue<char const *>(member.name));
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

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string GetLastError(JsonDocument const &json) {
	std::stringstream error;
	error << "Error at " << json.GetErrorOffset() << ": "
	      << rapidjson::GetParseError_En(json.GetParseError());
	return error.str();
}

std::string ToString(JsonValue const &value) {
	using namespace rapidjson;
	GenericStringBuffer<UTF8<> > buffer;
	Writer<GenericStringBuffer<UTF8<> > > writer(buffer);
	value.Accept(writer);

	std::string string = buffer.GetString();
	return buffer.GetString();
}

} // namespace JsonSchemaValidator
