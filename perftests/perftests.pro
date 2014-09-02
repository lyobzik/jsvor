include(../common.pri)

TEMPLATE = app
CONFIG += link_prl

QT -= core gui

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../thirdparty/rapidjson/include/ \
               ../include/ \
               ../tests_common/ \


LIBS += -L$${DESTDIR} -ltests_common -lwjelement -lwjreader -lre2 -lboost_timer -lboost_filesystem -lboost_system

HEADERS = WJValidator.h \


SOURCES = WJValidator.cc \
		  JsonPerformanceTests.cc \


PRE_TARGETDEPS += $${DESTDIR}/libtests_common.a
