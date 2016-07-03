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
