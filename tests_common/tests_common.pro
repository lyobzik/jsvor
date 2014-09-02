include(../common.pri)

TEMPLATE = lib
CONFIG += staticlib create_prl

QT -= core gui

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../include/

LIBS += -L$${DESTDIR} -ljsvor

HEADERS = Test.h \
          Validator.h \


SOURCES = Test.cc \
          Validator.cc \

PRE_TARGETDEPS += $${DESTDIR}/libjsvor.a
