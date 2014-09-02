include(../common.pri)

TEMPLATE = lib
CONFIG += staticlib create_prl

QT -= core gui

QMAKE_CXXFLAGS += -std=c++11

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
          types/JsonTypeImpl.h \
          types/PrimitiveTypes.h \
          types/CustomTypes.h \


SOURCES = JsonResolver.cc \
          JsonSchema.cc \
          JsonErrors.cc \
          JsonType.cc \
          RapidJsonHelpers.cc \
          Regex.cc \
          types/JsonTypeImpl.inl \
          types/PrimitiveTypes.cc \
          types/PrimitiveTypes.inl \
          types/CustomTypes.cc \
