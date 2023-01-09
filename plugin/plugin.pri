INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


# Input
HEADERS += \
    plugin/chapplication.h \
    plugin/VideoWatcher_plugin.h \
    plugin/VideoWatcher_widget.h \
    plugin/omsquery.h \
    plugin/omsqueryparser.h

SOURCES += \
    plugin/chapplication.cpp \
    plugin/VideoWatcher_plugin.cpp \
    plugin/VideoWatcher_widget.cpp \
    plugin/omsquery.cpp\
    plugin/omsqueryparser.cpp

contains(DEFINES, OUTPUT_APP):SOURCES += plugin/main.cpp
