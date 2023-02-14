QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x
CONFIG += resources_big

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(extra_settings/extra_settings.pri)

HEADERS += \
    src/widget/lineeditfind.h \
    src/widget/lineeditx.h \
    src/widget/propertycolor.h \
    src/widget/tableviewadd.h \
    src/datasource.h \
    src/treevideomodel.h \
    src/treevideoview.h \
    src/treevideoviewsearch.h \
    src/lds.h

SOURCES += \
    src/widget/lineeditfind.cpp \
    src/widget/lineeditx.cpp \
    src/widget/propertycolor.cpp \
    src/widget/tableviewadd.cpp \
    src/datasource.cpp \
    src/treevideomodel.cpp \
    src/treevideoview.cpp \
    src/treevideoviewsearch.cpp \
    src/lds.cpp

include(jsoncpp/jsoncpp.pri)

RESOURCES += \
    resources/res.qrc

INCLUDEPATH += src/widget
INCLUDEPATH += src


