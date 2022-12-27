DEPENDPATH  += $$PWD
INCLUDEPATH += \
    $$PWD/include/ \
    $$PWD/src/

HEADERS += \
    $$PWD/include/json/json.h \
    $$PWD/include/json/reader.h \
    $$PWD/include/json/value.h \
    $$PWD/include/json/writer.h

SOURCES += \
    $$PWD/src/lib_json/json_reader.cpp \
    $$PWD/src/lib_json/json_value.cpp \
    $$PWD/src/lib_json/json_writer.cpp
