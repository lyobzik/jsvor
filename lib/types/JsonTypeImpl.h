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

#include <string>
#include <vector>

#include <JsonDefs.h>

#include "../Defs.h"
#include "../JsonType.h"
#include "../RapidJsonHelpers.h"

namespace JsonSchemaValidator {

template <typename Type>
class JsonTypeImpl : public JsonType {
public:
	JsonTypeImpl(JsonValue const &schema, JsonResolverPtr const &resolver,
	             std::string const &path);

protected:
	typedef Type ValueType;

private:
	virtual void CheckEnumsRestrictions(JsonValue const &json, ValidationContext &context) const;

	JsonTypeProperty<std::vector<Type>> enum_;
}; // class JsonTypeImpl : public JsonType

} // namespace JsonSchemaValidator

#include "JsonTypeImpl.inl"
