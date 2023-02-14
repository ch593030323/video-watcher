INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += sql

include(jsoncpp/jsoncpp.pri)
include(ffmpeg/ffmpeg.pri)

HEADERS += \
    $$PWD/src/mainwindow.h \
    $$PWD/src/playalternatenewdialog.h \
    $$PWD/src/playformnewdialog.h \
    $$PWD/src/widget/lineeditfind.h \
    $$PWD/src/widget/lineeditx.h \
    $$PWD/src/widget/propertycolor.h \
    $$PWD/src/widget/tableviewadd.h \
    $$PWD/src/treevideomodel.h \
    $$PWD/src/treevideoviewsearch.h \
    $$PWD/src/treevideoviewsearchurl.h \
    $$PWD/src/videowidget.h \
    $$PWD/src/lds.h \
    $$PWD/src/videocontrolpanel.h \
    $$PWD/src/treevideoview.h \
    $$PWD/src/datasource.h \
    $$PWD/src/playthread.h \
    $$PWD/src/videolayout.h

SOURCES += \
    $$PWD/src/mainwindow.cpp \
    $$PWD/src/playalternatenewdialog.cpp \
    $$PWD/src/playformnewdialog.cpp \
    $$PWD/src/widget/lineeditfind.cpp \
    $$PWD/src/widget/lineeditx.cpp \
    $$PWD/src/widget/propertycolor.cpp \
    $$PWD/src/widget/tableviewadd.cpp \
    $$PWD/src/treevideomodel.cpp \
    $$PWD/src/treevideoviewsearch.cpp \
    $$PWD/src/treevideoviewsearchurl.cpp \
    $$PWD/src/videowidget.cpp \
    $$PWD/src/lds.cpp \
    $$PWD/src/videocontrolpanel.cpp \
    $$PWD/src/treevideoview.cpp \
    $$PWD/src/datasource.cpp \
    $$PWD/src/playthread.cpp \
    $$PWD/src/videolayout.cpp


FORMS += \
    $$PWD/src/mainwindow.ui \
    $$PWD/src/playalternatenewdialog.ui \
    $$PWD/src/playformnewdialog.ui

RESOURCES += \
    resources/res.qrc

INCLUDEPATH += $$PWD/src/widget/
INCLUDEPATH += $$PWD/src
