######################################################################
# Automatically generated by qmake (2.01a) ?? 5? 8 08:47:23 2019
######################################################################

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += OUTPUT_APP
DEFINES += OMS_DATASOURCE

contains(DEFINES, OUTPUT_APP) {
    TEMPLATE = app
    TARGET = VideoWatcher_app
} else {
    TEMPLATE = lib
    TARGET = $$(CPS_ENV)/plugin/sche/videoWatcher_plugin
}

CONFIG += plugin
QMAKE_CXXFLAGS += -std=c++0x

include(VideoWatcher_plugin_src.pri)

message($(CPS_ROOT))
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




include(plugin/plugin.pri)




#cp /usr/local/ffmpeg/lib/* ~/CPS_Project/ubuntu/thirdpartylib/


