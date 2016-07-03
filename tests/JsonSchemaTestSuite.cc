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

#include <memory>
#include <functional>

#include <gtest/gtest.h>

#include <JsonSchema.h>
#include <JsonErrors.h>
#include <JsonDefs.h>

#include <Test.h>
#include <Validator.h>

using namespace TestsCommon;
using namespace jsvor;

class JsonSchemaTestSuite : public ::testing::Test
{
protected:
	typedef std::function<bool (::Test const &)> TestFunction;

	static void CheckTestCase(JsonValue const &test_case, std::string const &filename);
	static void TestValidator(::Test const &test, TestFunction const &test_func);
}; // class JsonSchemaTestSuite : public ::testing::Test

void JsonSchemaTestSuite::TestValidator(::Test const &test, TestFunction const &test_func) {
	if (test_func(test)) {
		ASSERT_TRUE(test.GetExpectResult()) << test.GetName();
	} else {
		ASSERT_FALSE(test.GetExpectResult()) << test.GetName();
	}
}

TEST_F(JsonSchemaTestSuite, AllTests) {
	for (auto const &test : ::Test::GetTests()) {
		ValidatorPtr validator = std::make_shared<RJValidator>(test.GetSchema());

		TestValidator(test, [validator](::Test const &test) -> bool {
			return validator->Validate(test.GetInspectedDocument());
		});
		TestValidator(test, [validator](::Test const &test) -> bool {
			validator->Load(test.GetInspectedDocument());
			return validator->Validate();
		});
	}
}
