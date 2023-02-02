QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++0x

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
TARGET = GD_DataCompare

include($(CPS_ROOT)/CPS/compile/cps_common.pri)
INCLUDEPATH += $(CPS_ROOT)/CPS/include
#INCLUDEPATH += $(CPS_ROOT)/CPS/src/app/sche/schedraw/include
INCLUDEPATH += $(CPS_ROOT)/CPS/include/qwt
INCLUDEPATH += $(CPS_ENV)/include/qt

unix {
    CONFIG(debug, debug|release) {
        LIBS += -ldatabase -lmetroApp
        LIBS += -lACE
        #LIBS += -lOperationLog
        #LIBS += -lUserMS
        #LIBS += -lORMDatabase
    } else {
        LIBS += -ldatabase -lmetroApp -lOperationLog -lUserMS
        LIBS += -lACE
        LIBS += -lOperationLog
        LIBS += -lUserMS
        #LIBS += -lORMDatabase
##########################################################
LIBS += -lORMDatabase -lstatecliwrap
#LIBS += -lstateclientd
LIBS += -llog4cplus -lNetViewWnd \
        -lCPS_NetService -lLog_Interface -lapplication \
         -ldatabase -ltolerance -lCPS_XBus_MBIAPI
LIBS += -pthread
LIBS +=  -lm -lnsl
##########################################################
# LIBS += -lQtCore -lQtGui -lQtNetwork
##########################################################
LIBS += -lNetViewWnd
##########################################################
    }

    contains(DEFINES, CPS_PROJECT20) {
        LIBS += -loms
    }
    else{
        LIBS += -lomcd
    }

    VERSION = 1.4.0
}

win32 {
    #CONFIG                  += console
    QMAKE_CXXFLAGS          += -DSCHECURVE_BUILD_DLL   -DWIN32_LEAN_AND_MEAN
    #RC_FILE                 =  schebarplugin.rc
    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK = $$QMAKE_COPY debug\\*.dll $(CPS_ENV)\\plugin\\sche \
                          & $$QMAKE_COPY debug\\*.lib $(CPS_ENV)\\plugin\\sche

        LIBS += libHistClientd.lib  libdatabased.lib \
                                libormd.lib  ACEd.lib PMSd.lib schedrawd.lib qwt.lib DBComponents.lib SacPlot.lib $(QTDIR)\\lib\\Qt3Support4.lib
    }

    else{
        QMAKE_POST_LINK = $$QMAKE_COPY release\\*.dll $(CPS_ENV)\\plugin\\sche \
                          & $$QMAKE_COPY release\\*.lib $(CPS_ENV)\\plugin\\sche

        LIBS += libHistClient.lib  libdatabase.lib liborm.lib \
                                ACE.lib PMS.lib schedraw.lib qwt.lib DBComponents.lib SacPlot.lib  $(QTDIR)\\lib\\Qt3Support4.lib
    }
}

SOURCES += \
    DataCompare/main.cpp \
    plugin/omsquery.cpp \
    plugin/omsqueryparser.cpp \
    plugin/chapplication.cpp

HEADERS += \
    plugin/omsquery.h \
    plugin/omsqueryparser.h \
    plugin/chapplication.h

INCLUDEPATH += plugin



