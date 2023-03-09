INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
QT += sql

include(jsoncpp/jsoncpp.pri)
include(ffmpeg/ffmpeg.pri)

HEADERS += \
    $$PWD/src/mainwindow.h \
    $$PWD/src/playalternatenewdialog.h \
    $$PWD/src/playformnewdialog.h \
    $$PWD/src/videocell.h \
    $$PWD/src/videowidget.h \
    $$PWD/src/widget/cameracontrolpanel.h \
    $$PWD/src/widget/lineeditfind.h \
    $$PWD/src/widget/lineeditx.h \
    $$PWD/src/widget/propertycolor.h \
    $$PWD/src/widget/tableviewadd.h \
    $$PWD/src/treevideomodel.h \
    $$PWD/src/treevideoviewsearch.h \
    $$PWD/src/treevideoviewsearchurl.h \
    $$PWD/src/lds.h \
    $$PWD/src/videocontrolpanel.h \
    $$PWD/src/treevideoview.h \
    $$PWD/src/datasource.h \
    $$PWD/src/playthread.h \
    $$PWD/src/videolayout.h \
    $$PWD/src/videoinspanel.h \
    $$PWD/src/widget/iconbutton.h \
    $$PWD/src/widget/tipslider.h \
    $$PWD/src/mainvideowidget.h \
    $$PWD/src/mainplayback.h \
    $$PWD/src/mainsystemsetingsalter.h \
    $$PWD/src/mainsystemsetingsform.h \
    $$PWD/src/mainsystemsetings.h \
    $$PWD/src/widget/lds_tableview.h \
    $$PWD/src/widget/editedwidgetmapper.h \
    $$PWD/src/widget/lds_checkbox_phonestyle.h \
    $$PWD/src/mainsystemsetingssnap.h \
    $$PWD/src/global.h \
    $$PWD/src/widget/autocloselabel.h \
    $$PWD/src/mainsystemsetingssnaplistview.h\
    $$PWD/src/mainsystemsetingsurl.h

SOURCES += \
    $$PWD/src/mainwindow.cpp \
    $$PWD/src/playalternatenewdialog.cpp \
    $$PWD/src/playformnewdialog.cpp \
    $$PWD/src/videocell.cpp \
    $$PWD/src/videowidget.cpp \
    $$PWD/src/widget/cameracontrolpanel.cpp \
    $$PWD/src/widget/lineeditfind.cpp \
    $$PWD/src/widget/lineeditx.cpp \
    $$PWD/src/widget/propertycolor.cpp \
    $$PWD/src/widget/tableviewadd.cpp \
    $$PWD/src/treevideomodel.cpp \
    $$PWD/src/treevideoviewsearch.cpp \
    $$PWD/src/treevideoviewsearchurl.cpp \
    $$PWD/src/lds.cpp \
    $$PWD/src/videocontrolpanel.cpp \
    $$PWD/src/treevideoview.cpp \
    $$PWD/src/datasource.cpp \
    $$PWD/src/playthread.cpp \
    $$PWD/src/videolayout.cpp \
    $$PWD/src/videoinspanel.cpp \
    $$PWD/src/widget/iconbutton.cpp \
    $$PWD/src/widget/tipslider.cpp \
    $$PWD/src/mainvideowidget.cpp \
    $$PWD/src/mainplayback.cpp \
    $$PWD/src/mainsystemsetingsalter.cpp \
    $$PWD/src/mainsystemsetingsform.cpp \
    $$PWD/src/mainsystemsetings.cpp \
    $$PWD/src/widget/lds_tableview.cpp \
    $$PWD/src/widget/editedwidgetmapper.cpp \
    $$PWD/src/widget/lds_checkbox_phonestyle.cpp \
    $$PWD/src/mainsystemsetingssnap.cpp \
    $$PWD/src/global.cpp \
    $$PWD/src/widget/autocloselabel.cpp \
    $$PWD/src/mainsystemsetingssnaplistview.cpp \
    $$PWD/src/mainsystemsetingsurl.cpp

FORMS += \
    $$PWD/src/mainwindow.ui \
    $$PWD/src/playalternatenewdialog.ui \
    $$PWD/src/playformnewdialog.ui \
    $$PWD/src/videoinspanel.ui \
    $$PWD/src/mainvideowidget.ui \
    $$PWD/src/mainplayback.ui \
    $$PWD/src/mainsystemsetingsalter.ui \
    $$PWD/src/mainsystemsetingsform.ui \
    $$PWD/src/mainsystemsetings.ui\
    $$PWD/src/mainsystemsetingssnap.ui \
    $$PWD/src/mainsystemsetingsurl.ui

RESOURCES += \
    resources/res.qrc

INCLUDEPATH += $$PWD/src/widget/
INCLUDEPATH += $$PWD/src

