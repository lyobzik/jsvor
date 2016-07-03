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

#include "Regex.h"

#include <memory>

namespace JsonSchemaValidator {

Regex::Regex(char const *pattern)
	: pattern_(pattern) {
}

Regex::~Regex() {
}

char const *Regex::Pattern() const {
	return pattern_.c_str();
}

} // namespace JsonSchemaValidator

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(USE_STD_REGEX)

#include <regex>

namespace JsonSchemaValidator {

class StdRegex : public Regex {
public:
	StdRegex(char const *pattern);
	virtual ~StdRegex();

	bool IsCorrespond(char const *value) const;

private:
	std::shared_ptr<std::regex> impl_;
}; // class StdRegex : public Regex

StdRegex::StdRegex(char const *pattern)
	: Regex(pattern)
	, impl_(std::make_shared<std::regex>(pattern)) {
}

StdRegex::~StdRegex() {
}

bool StdRegex::IsCorrespond(char const *value) const {
	return std::regex_search(value, *impl_);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RegexPtr Regex::Create(char const *pattern) {
	return std::make_shared<StdRegex>(pattern);
}

} // namespace JsonSchemaValidator

#else // defined(USE_STD_REGEX)

#include <re2/re2.h>

namespace JsonSchemaValidator {

class Re2Regex : public Regex {
public:
	Re2Regex(char const *pattern);
	virtual ~Re2Regex();

	bool IsCorrespond(char const *value) const;

private:
	std::shared_ptr<re2::RE2> impl_;
}; // class Re2Regex : public Regex

Re2Regex::Re2Regex(char const *pattern)
	: Regex(pattern)
	, impl_(std::make_shared<re2::RE2>(pattern)) {
}

Re2Regex::~Re2Regex() {
}

bool Re2Regex::IsCorrespond(char const *value) const {
	return RE2::PartialMatch(value, *impl_);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RegexPtr Regex::Create(char const *pattern) {
	return std::make_shared<Re2Regex>(pattern);
}

} // namespace JsonSchemaValidator

#endif // defined(USE_STD_REGEX)

