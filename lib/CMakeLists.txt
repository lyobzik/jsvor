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
	CoreSchema.inl
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
