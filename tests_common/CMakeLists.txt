cmake_minimum_required(VERSION 2.8)

set(PROJECT tests_common)
set(PROJECT_TYPE library)

include(../CMakeLists_header.txt)

set(SOURCES
	   Test.cc
	   Validator.cc
)

set(HEADERS
		Test.h
		Validator.h
)

find_package(Boost COMPONENTS system filesystem REQUIRED)
set(LIBS ${LIBS} ${Boost_LIBRARIES})

set(LIBS ${LIBS} jsvor)

include(../CMakeLists_footer.txt)
