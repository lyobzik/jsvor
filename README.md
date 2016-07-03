JsonSchemaValidator
===================

Simple validator of json, based on Json-Schema (draft03).

Usage
-----
```c
#include <iostream>

#include <JsonSchema.h>
#include <JsonErrors.h>
#include <JsonDefs.h>

namespace jsvor = JsonSchemaValidator;

void Print(std::string const &context, std::string const &value,
           jsvor::ValidationResult const &result);

int main(int /*argc*/, char * /*argv*/[]) {
	using namespace jsvor;

	// JsonSchema can be created from text of json-document (exception 'IncorrectJson' will be
	// thrown on error of parsing this text) or from already parsed json-document (in this case
	// lifetime of json-document must be longer than lifetime of 'JsonSchema').
	// Exception 'IncorrectSchema' will be thrown if some part of document has not satisfied
	// to restrictions on JsonSchema.
	JsonSchema schema(R"({"properties": {"data": {
                             "type": "integer", "minimum": 10, "maximum": 100, "divisibleBy": 2}
                         } })");

	for (auto const &value : { R"({"data": 0})", R"({"data": 20})",
	                           R"({"data": 21})", R"({"data": 101})" }) {
		// To validate json-document, you can call the method 'Validate' and then handle outgoing
		// parameter 'result'. From this parameter, you can get overall result of validation,
		// error code and detailed error description.
		ValidationResult result;
		schema.Validate(value, result);
		Print("Validate(1)", value, result);

		// Also you can use version of method 'Validate' with exception. On error this method
		// throws exception 'IncorrectDocument' from which you can get validation error.
		try {
			schema.Validate(value);
		}
		catch (IncorrectDocument const &error) {
			Print("Validate(2)", value, error.GetResult());
		}
	}
}

void Print(std::string const &context, std::string const &value,
           jsvor::ValidationResult const &result) {
	std::cout << std::boolalpha << context << " <" << value << ">:"
	          << "(" << result << ") [" << static_cast<int>(result.Error()) << "] "
	          << result.ErrorDescription() << "." << std::noboolalpha << std::endl;
}
```
Output:
```
Validate(1) <{"data": 0}>:(false) [5] Inspected document not satisfied restriction on minimum.
Validate(2) <{"data": 0}>:(false) [5] Inspected document not satisfied restriction on minimum.
Validate(1) <{"data": 20}>:(true) [0] None.
Validate(1) <{"data": 21}>:(false) [7] Inspected document not satisfied restriction on divisible.
Validate(2) <{"data": 21}>:(false) [7] Inspected document not satisfied restriction on divisible.
Validate(1) <{"data": 101}>:(false) [6] Inspected document not satisfied restriction on maximum.
Validate(2) <{"data": 101}>:(false) [6] Inspected document not satisfied restriction on maximum.
```

Dependencies
------------

* [RapidJSON](https://github.com/miloyip/rapidjson) used for parsing json (**included**).
* [JSON-Schema-Test-Suite](https://github.com/json-schema/JSON-Schema-Test-Suite) used for tests (**included**).
* [re2](https://code.google.com/p/re2/) used by default for working with regex. If you define `USE_STD_REGEX` for working with regex will be used `std::regex`.
* [WJElement](https://github.com/netmail-open/wjelement) used in project `perftests` for comparing of working speed.
* [cmake](https://cmake.org/) and [qmake](https://en.wikipedia.org/wiki/Qmake) are used for build.
