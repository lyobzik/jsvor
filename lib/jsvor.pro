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

include(../common.pri)

TEMPLATE = lib
CONFIG += staticlib create_prl

QT -= core gui

INCLUDEPATH += ../include/

HEADERS = ../include/JsonResolver.h \
          ../include/JsonErrors.h \
          ../include/JsonSchema.h \
          ../include/JsonDefs.h \
          RapidJsonDefs.h \
          RapidJsonHelpers.h \
          Defs.h \
          JsonType.h \
          Regex.h \
          ValidationContext.h \
          ValidationContext.inl \
          types/JsonTypeImpl.h \
          types/PrimitiveTypes.h \
          types/CustomTypes.h \


SOURCES = JsonResolver.cc \
          JsonSchema.cc \
          JsonErrors.cc \
          JsonType.cc \
          RapidJsonHelpers.cc \
          Regex.cc \
          ValidationContext.cc \
          types/JsonTypeImpl.inl \
          types/PrimitiveTypes.cc \
          types/PrimitiveTypes.inl \
          types/CustomTypes.cc \
