linux:QMAKE_CXXFLAGS += -isystem$${PWD}/thirdparty/rapidjson/include/
INCLUDEPATH += $${PWD}/thirdparty/rapidjson/include/

CONFIG(debug, debug|release) {
    OUTPUT_DIR = ../Output/Debug
} else {
    OUTPUT_DIR = ../Output/Release
}

OBJECTS_DIR = $${OUTPUT_DIR}/Obj
DESTDIR = $${OUTPUT_DIR}

QMAKE_CLEAN += Makefile*

QMAKE_CXXFLAGS += -std=c++11
