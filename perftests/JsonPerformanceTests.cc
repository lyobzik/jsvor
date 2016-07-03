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
#include <sstream>

#include <JsonSchema.h>
#include <JsonErrors.h>
#include <JsonDefs.h>

#include <boost/timer/timer.hpp>

#include <Test.h>

#include "WJValidator.h"

double TestValidator(ValidatorPtr const &validator, Test const &test) {
	static int const tests_count = 10000;

	validator->Load(test.GetInspectedDocument());

	boost::timer::cpu_timer timer;
	for (int i = 0; i < tests_count; ++i) {
		validator->Validate();
	}
	return static_cast<double>(timer.elapsed().wall) / tests_count;
}

int main(int /*argc*/, char * /*argv*/[]) {
	std::stringstream report_stream;
	double sum_rjtime = 0, sum_wjtime = 0;
	for (auto const &test : ::Test::GetTests()) {
		std::cout << test.GetName() << std::endl;
		ValidatorPtr rjvalidator = std::make_shared<RJValidator>(test.GetSchema());
		ValidatorPtr wjvalidator = std::make_shared<WJValidator>(test.GetSchema());

		double rjtime = TestValidator(rjvalidator, test);
		double wjtime = TestValidator(wjvalidator, test);

		if (wjtime / rjtime < 20) {
			report_stream << test.GetName() << ": " << rjtime << "   " << wjtime << std::endl;
		}

		sum_rjtime += rjtime;
		sum_wjtime += wjtime;
	}
	std::cout << std::endl << report_stream.str() << std::endl;
	std::cout << "SUM: " << " " << sum_rjtime << "   " << sum_wjtime << std::endl;
	std::cout << sum_wjtime / sum_rjtime << std::endl;
}
