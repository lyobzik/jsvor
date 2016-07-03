# Copyright 2016 lyobzik
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 2.8)

set(PROJECT perftests)
set(PROJECT_TYPE executable)

include(../CMakeLists_header.txt)

INCLUDE_DIRECTORIES(
	   ../tests_common/
)

set(SOURCES
	   WJValidator.cc
	   JsonPerformanceTests.cc
)

set(HEADERS
		WJValidator.h
)

find_package(Boost COMPONENTS timer REQUIRED)
set(LIBS ${LIBS} ${Boost_LIBRARIES})

find_library(WJELEMENT wjelement)
if (NOT WJELEMENT)
	return()
endif(NOT WJELEMENT)
set(LIBS ${LIBS} ${WJELEMENT})

find_library(WJREADER wjreader)
if (NOT WJREADER)
	return()
endif(NOT WJREADER)
set(LIBS ${LIBS} ${WJREADER})

set(LIBS ${LIBS} tests_common)

include(../CMakeLists_footer.txt)
