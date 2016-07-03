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

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <JsonDefs.h>
#include <JsonSchema.h>
#include <JsonErrors.h>
#include <JsonResolver.h>

// Resolver and typedef for jsvor.
namespace jsvor = JsonSchemaValidator;

class SimpleResolver : public jsvor::JsonResolver {
public:
	explicit SimpleResolver()
	: schemas_() {
	}

	virtual jsvor::JsonSchemaPtr Resolve(const std::string &ref) const {
		auto it = schemas_.find(ref);
		return it != schemas_.end() ? it->second : jsvor::JsonSchemaPtr();
	}

	void AddSchema(const std::string &schema_path, const jsvor::JsonSchemaPtr &schema) {
		schemas_.insert({schema_path, schema});
	}

private:
	std::map<std::string, jsvor::JsonSchemaPtr> schemas_;
};

typedef std::vector<jsvor::JsonSchemaPtr> JsonSchemas;
typedef std::shared_ptr<SimpleResolver> SimpleResolverPtr;

// Show error and help.
template <typename Value>
void Show(std::ostream &os, const Value& value) {
	os << value;
}

template <typename Value, typename... Values>
void Show(std::ostream &os, const Value &value, const Values&... values) {
	Show(os, value);
	Show(os, values...);
}

void Usage() {
	Show(std::cout, "Usage: Validator [SCHEMA]... MAIN_SCHEMA JSON");
	std::cout << std::endl;
	exit(EXIT_SUCCESS);
}

template <typename... Args>
void ShowError(const Args&... args) {
	Show(std::cerr, args...);
	std::cerr << std::endl;
	exit(EXIT_FAILURE);
}

// Load files and schemas.
std::string LoadFileContent(const std::string &file_path) {
	std::ifstream file(file_path.c_str());
	if (not file.is_open()) {
		ShowError("Cannot open file ", file_path);
	}
	return std::string((std::istreambuf_iterator<char>(file)),
	                   std::istreambuf_iterator<char>());
}

std::string GetFileName(const std::string &file_path) {
	std::string::size_type last_slash = file_path.find_last_of("/\\");
	if (last_slash != std::string::npos) {
		return file_path.substr(last_slash + 1);
	}
	return file_path;
}

jsvor::JsonSchemaPtr LoadSchema(const std::string &schema_path,
                                const SimpleResolverPtr &resolver) {
	const std::string content = LoadFileContent(schema_path);
	try {
		auto schema = std::make_shared<jsvor::JsonSchema>(content, resolver);
		resolver->AddSchema(GetFileName(schema_path), schema);
		return schema;
	}
	catch (const jsvor::Error &ex) {
		ShowError("Could not load schema from ", schema_path, ": ", ex.what());
	}
	return jsvor::JsonSchemaPtr();
}

int main(int argc, char *argv[]) {
	// Parse arguments.
	if (argc < 3) {
		Usage();
	}

	std::vector<std::string> schema_paths;
	for (int arg_index = 1; arg_index < argc - 2; ++arg_index) {
		schema_paths.push_back(std::string(argv[arg_index]));
	}
	std::string main_schema_path = argv[argc - 2];
	std::string json_path = argv[argc - 1];

	// Load schemas.
	auto resolver = std::make_shared<SimpleResolver>();

	for (const auto& schema_path: schema_paths) {
		LoadSchema(schema_path, resolver);
	}
	auto main_schema = LoadSchema(main_schema_path, resolver);

	// Load and validate file.
	auto json = LoadFileContent(json_path);
	try {
		main_schema->Validate(json);
	}
	catch (const jsvor::IncorrectDocument &error) {
		ShowError("Incorrect json document: ", error.what());
	}
	exit(EXIT_SUCCESS);
}
