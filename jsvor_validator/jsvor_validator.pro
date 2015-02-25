include(../common.pri)

TEMPLATE = app
CONFIG += link_prl

QT -= core gui

INCLUDEPATH += ../thirdparty/rapidjson/include/ \
               ../include/ \


LIBS += -L$${DESTDIR} -ljsvor -lre2


SOURCES = JsvorValidator.cc \


PRE_TARGETDEPS += $${DESTDIR}/libjsvor.a
