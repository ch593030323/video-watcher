INCLUDEPATH += $$PWD
INCLUDEPATH += plugin
DEPENDPATH += $$PWD


# Input
HEADERS += \
    simulation/simulation_plugin.h \
    simulation/simulation_widget.h \
    plugin/chapplication.h \
    plugin/omsquery.h \
    plugin/omsqueryparser.h

SOURCES += \
    simulation/simulation_plugin.cpp \
    simulation/simulation_widget.cpp \
    plugin/chapplication.cpp \
    plugin/omsquery.cpp \
    plugin/omsqueryparser.cpp

contains(DEFINES, OUTPUT_APP):SOURCES += simulation/main.cpp
