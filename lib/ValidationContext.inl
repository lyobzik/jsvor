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

#include "ValidationContext.h"

#include <JsonErrors.h>

#include "Defs.h"

namespace JsonSchemaValidator {

template <typename ElementName>
PathHolder<ElementName>::PathHolder(ElementName const &name, ValidationContext &context)
	: need_set_(true)
	, name_(name)
	, context_(context) {
}

template <typename ElementName>
PathHolder<ElementName>::~PathHolder() {
	if (need_set_) {
		context_.GetResult().AddPath(ElementNameToString());
	}
}

template <typename ElementName>
void PathHolder<ElementName>::Reset() {
	need_set_ = false;
}

} // namespace JsonSchemaValidator
