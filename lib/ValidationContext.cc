#include "ValidationContext.h"

namespace JsonSchemaValidator {

namespace {

bool EndsWith(std::string const &string, char symbol) {
	return !string.empty() && string[string.size() - 1] == symbol;
}

bool StartsWith(std::string const &string, char symbol) {
	return !string.empty() && string[0] == symbol;
}

void JoinPath(std::string const &tail_path, std::string &result_path) {
	static char const slash = '/';

	if (!result_path.empty() &&
	    StartsWith(tail_path, slash) && tail_path.find_first_not_of(slash) == std::string::npos) {
		return;
	}

	if (!EndsWith(result_path, slash)) {
		result_path.append(1, slash);
	}

	size_t tail_start_pos = StartsWith(tail_path, slash) ? 1 : 0;
	result_path.append(tail_path.begin() + tail_start_pos, tail_path.end());
}

} // namespace

ValidationContext::ValidationContext(ValidationResult &result)
	: path_parts_()
	, result_(result) {
}

void ValidationContext::AddPath(std::string const &path) {
	path_parts_.push_back(path);
}

void ValidationContext::RemovePath() {
	path_parts_.pop_back();
}

ValidationResult& ValidationContext::GetResult() {
	return result_;
}

std::string ValidationContext::GetPath(std::string const &path) const {
	if (path_parts_.empty()) {
		return path;
	}

	std::string result_path;
	for (auto const &path_part: path_parts_) {
		JoinPath(path_part, result_path);
	}
	JoinPath(path, result_path);
	return result_path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ResolvePathHolder::ResolvePathHolder(std::string const &path, ValidationContext &context)
	: context_(context) {
	context_.AddPath(path);
}

ResolvePathHolder::~ResolvePathHolder() {
	context_.RemovePath();
}

} // namespace JsonSchemaValidator
