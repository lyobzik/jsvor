#pragma once

#include <rapidjson/document.h>

#include "../include/JsonDefs.h"

namespace JsonSchemaValidator {

typedef rapidjson::SizeType JsonSizeType;
typedef rapidjson::Value::Member JsonValueMember;
typedef rapidjson::Value::ConstMemberIterator JsonValueConstMemberIterator;

} // namespace JsonSchemaValidator
