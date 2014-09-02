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
