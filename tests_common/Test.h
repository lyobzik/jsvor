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


