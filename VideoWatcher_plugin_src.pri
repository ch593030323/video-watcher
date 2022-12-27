INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += sql

include(jsoncpp/jsoncpp.pri)
include(ffmpeg/ffmpeg.pri)

HEADERS += \
    $$PWD/src/mainwindow.h \
    $$PWD/src/playalternatenewdialog.h \
    $$PWD/src/playformnewdialog.h \
    $$PWD/src/treevideomodel.h \
    $$PWD/src/videowidget.h \
    $$PWD/src/lds.h \
    $$PWD/src/videocontrolpanel.h \
    $$PWD/src/treevideoview.h

SOURCES += \
    $$PWD/src/mainwindow.cpp \
    $$PWD/src/playalternatenewdialog.cpp \
    $$PWD/src/playformnewdialog.cpp \
    $$PWD/src/treevideomodel.cpp \
    $$PWD/src/videowidget.cpp \
    $$PWD/src/lds.cpp \
    $$PWD/src/videocontrolpanel.cpp \
    $$PWD/src/treevideoview.cpp


FORMS += \
    $$PWD/src/mainwindow.ui \
    $$PWD/src/playalternatenewdialog.ui \
    $$PWD/src/playformnewdialog.ui

RESOURCES += \
    resources/res.qrc
