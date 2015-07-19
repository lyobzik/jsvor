cmake_minimum_required(VERSION 2.8)

set(PROJECT jsvor)
set(PROJECT_TYPE library)

include(../CMakeLists_header.txt)

set(SOURCES
	   JsonResolver.cc
	   JsonSchema.cc
	   JsonErrors.cc
	   JsonType.cc
	   RapidJsonHelpers.cc
	   Regex.cc
	   ValidationContext.cc
	   types/JsonTypeImpl.inl
	   types/PrimitiveTypes.cc
	   types/PrimitiveTypes.inl
	   types/CustomTypes.cc
)

set(HEADERS
		../include/JsonResolver.h
		../include/JsonErrors.h
		../include/JsonSchema.h
		../include/JsonDefs.h
		RapidJsonDefs.h
		RapidJsonHelpers.h
		Defs.h
		JsonType.h
		Regex.h
		ValidationContext.h
		ValidationContext.inl
		types/JsonTypeImpl.h
		types/PrimitiveTypes.h
		types/CustomTypes.h
)

if(NOT USE_STD_REGEX)
	find_library(RE2 re2)
	set(LIBS ${LIBS} ${RE2})

	find_package(Threads) # linking bug in re2
	set(LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
else()
	ADD_DEFINITIONS(-DUSE_STD_REGEX)
endif()

include(../CMakeLists_footer.txt)
