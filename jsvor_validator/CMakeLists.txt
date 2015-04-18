cmake_minimum_required(VERSION 2.8)

set(PROJECT jsvor_validator)
set(PROJECT_TYPE executable)

include(../CMakeLists_header.txt)

set(SOURCES
	   JsvorValidator.cc
)

set(HEADERS
)

set(LIBS ${LIBS} jsvor)

include(../CMakeLists_footer.txt)
