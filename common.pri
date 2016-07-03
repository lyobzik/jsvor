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

linux:QMAKE_CXXFLAGS += -isystem$${PWD}/thirdparty/rapidjson/include/
INCLUDEPATH += $${PWD}/thirdparty/rapidjson/include/

CONFIG(release, debug|release) {
    OUTPUT_DIR = ../Output/Release
} else {
    OUTPUT_DIR = ../Output/Debug
}

OBJECTS_DIR = $${OUTPUT_DIR}/Obj
DESTDIR = $${OUTPUT_DIR}

QMAKE_CLEAN += Makefile*

QMAKE_CXXFLAGS += -std=c++11
