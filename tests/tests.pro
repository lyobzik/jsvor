include(../common.pri)

TEMPLATE = app
CONFIG += link_prl

QT -= core gui

INCLUDEPATH += ../thirdparty/rapidjson/include/ \
               ../include/ \
               ../tests_common/ \


LIBS += -L$${DESTDIR} -ltests_common -lre2 -lboost_filesystem -lboost_system -lgtest -lgtest_main

SOURCES = JsonSchemaTestSuite.cc \


PRE_TARGETDEPS += $${DESTDIR}/libtests_common.a

QMAKE_POST_LINK += $${DESTDIR}/$${TARGET}
