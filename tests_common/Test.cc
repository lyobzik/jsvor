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

#include "Test.h"

#include <sstream>
#include <fstream>

#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <boost/algorithm/string/predicate.hpp>

#include "../lib/RapidJsonDefs.h"

namespace TestsCommon {

namespace fs = boost::filesystem;

namespace {

std::string ToString(JsonSchemaValidator::JsonValue const &value) {
	using namespace rapidjson;
	GenericStringBuffer<UTF8<> > buffer;
	Writer<GenericStringBuffer<UTF8<> > > writer(buffer);
	value.Accept(writer);

	std::string string = buffer.GetString();
	return buffer.GetString();
}

std::vector<fs::path> GetTestFiles()
{
	fs::directory_iterator it("../thirdparty/json_schema_test_suite/tests/draft3"), endIt;
	std::vector<fs::path> test_files;
	std::copy_if(it, endIt, std::back_inserter(test_files), [](fs::directory_entry const &entry) -> bool {
		return fs::is_regular_file(entry) &&
		       boost::algorithm::ends_with(entry.path().string(), ".json") &&
		       entry.path().filename().string() != "ref.json";
	});
	return test_files;
}

void LoadTestCases(fs::path const &path_to_file, JsonSchemaValidator::JsonDocument &test_case) {
	std::ifstream test_file(path_to_file.string());
	std::istreambuf_iterator<char> it(test_file), endIt;
	std::string test_file_content(it, endIt);

	test_case.Parse<0>(test_file_content.c_str());
}

std::string GetStringTestData(JsonSchemaValidator::JsonValue const &test_data) {
	std::stringstream ss;
	ss << std::boolalpha << std::fixed << "{\"data\": ";
	if (test_data.IsObject() || test_data.IsArray()) ss << ToString(test_data);
	else if (test_data.IsString()) ss << "\"" << test_data.GetString() << "\"";
	else if (test_data.IsDouble()) ss << test_data.GetDouble();
	else if (test_data.IsNumber()) ss << test_data.GetInt64();
	else if (test_data.IsBool()) ss << test_data.GetBool();
	else if (test_data.IsNull()) ss << "null";
	ss << "}";
	return ss.str();
}

} // namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
Test::Test(std::string const &name, std::string const &schema,
           std::string const &inspected_document, bool expect_result)
	: name_(name)
	, schema_(schema)
	, inspected_document_(inspected_document)
	, expect_result_(expect_result) {
}

std::string Test::GetName() const {
	return name_;
}

std::string Test::GetSchema() const {
	return schema_;
}

std::string Test::GetInspectedDocument() const {
	return inspected_document_;
}

bool Test::GetExpectResult() const {
	return expect_result_;
}

Tests Test::GetTests() {
	Tests result;
	using namespace JsonSchemaValidator;
	for (auto const &file : GetTestFiles()) {
		JsonDocument test_cases;
		LoadTestCases(file, test_cases);
		assert(!test_cases.HasParseError());
		assert(test_cases.IsArray());

		for (JsonSizeType case_index = 0; case_index < test_cases.Size(); ++case_index) {
			JsonValue const &test_case(test_cases[case_index]);
			assert(test_case.HasMember("description"));
			assert(test_case.HasMember("schema"));
			assert(test_case.HasMember("tests"));
			assert(test_case["description"].IsString());
			assert(test_case["tests"].IsArray());

			std::string schema = ToString(test_case["schema"]);
			std::string description = test_case["description"].GetString();
			JsonValue const &tests(test_case["tests"]);
			for (JsonSizeType test_index = 0; test_index < tests.Size(); ++test_index) {
				JsonValue const &test(tests[test_index]);
				assert(test.HasMember("description"));
				assert(test.HasMember("data"));
				assert(test.HasMember("valid"));
				assert(test["description"].IsString());
				assert(test["valid"].IsBool());

				std::string test_name = file.filename().string() + "/" + description +
				                        "/" + test["description"].GetString();
				result.emplace_back(test_name, schema, GetStringTestData(test["data"]),
				                    test["valid"].GetBool());
			}
		}
	}
	return result;
}

} // namespace TestsCommon
