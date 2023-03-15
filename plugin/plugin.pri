INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


# Input
HEADERS += \
    plugin/chapplication.h \
    plugin/VideoWatcher_plugin.h \
    plugin/VideoWatcher_widget.h \
    plugin/omsquery.h \
    plugin/omsqueryparser.h \
    extra_settings/treevideoviewsearchsettings.h

SOURCES += \
    plugin/chapplication.cpp \
    plugin/VideoWatcher_plugin.cpp \
    plugin/VideoWatcher_widget.cpp \
    plugin/omsquery.cpp \
    plugin/omsqueryparser.cpp \
    extra_settings/treevideoviewsearchsettings.cpp

contains(DEFINES, OUTPUT_APP):SOURCES += plugin/main.cpp

INCLUDEPATH += extra_settings
