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

#include <string>
#include <vector>
#include <memory>

#include <JsonDefs.h>

#include <boost/filesystem.hpp>

namespace TestsCommon {

class Test;
typedef std::vector<Test> Tests;

class Test {
public:
	Test(std::string const &name, std::string const &schema,
	     std::string const &inspected_document, bool expect_result);

	std::string GetName() const;
	std::string GetSchema() const;
	std::string GetInspectedDocument() const;
	bool GetExpectResult() const;

	static Tests GetTests();

private:
	std::string name_;
	std::string schema_;
	std::string inspected_document_;
	bool expect_result_;
}; // class Test

} // namespace TestsCommon


