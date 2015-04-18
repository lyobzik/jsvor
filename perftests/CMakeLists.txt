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
set(LIBS ${LIBS} ${WJELEMENT})

find_library(WJREADER wjreader)
set(LIBS ${LIBS} ${WJREADER})

set(LIBS ${LIBS} tests_common)

include(../CMakeLists_footer.txt)
