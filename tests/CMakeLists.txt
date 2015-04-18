cmake_minimum_required(VERSION 2.8)

set(PROJECT tests)
set(PROJECT_TYPE executable)

include(../CMakeLists_header.txt)

INCLUDE_DIRECTORIES(
	   ../tests_common/
)

set(SOURCES
	   JsonSchemaTestSuite.cc
)

set(HEADERS
)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GTEST_BOTH_LIBRARIES})

set(LIBS ${LIBS} tests_common)

include(../CMakeLists_footer.txt)
