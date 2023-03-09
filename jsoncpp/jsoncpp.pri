DEPENDPATH  += $$PWD
INCLUDEPATH += \
    $$PWD/include/ \
    $$PWD/src/ \
    $$PWD/

HEADERS += \
    $$PWD/include/json/json.h \
    $$PWD/include/json/reader.h \
    $$PWD/include/json/value.h \
    $$PWD/include/json/writer.h\
    $$PWD/jsoncpppath.h

SOURCES += \
    $$PWD/src/lib_json/json_reader.cpp \
    $$PWD/src/lib_json/json_value.cpp \
    $$PWD/src/lib_json/json_writer.cpp \
    $$PWD/jsoncpppath.cpp
