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
