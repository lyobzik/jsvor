#include "Regex.h"

#include <memory>

namespace JsonSchemaValidator {

Regex::Regex() {
}

Regex::~Regex() {
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
	: Regex()
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
	: Regex()
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

