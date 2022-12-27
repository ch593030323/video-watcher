HEADERS += $$PWD/ffmpeghead.h
HEADERS += $$PWD/ffmpeg.h
SOURCES += $$PWD/ffmpeg.cpp
DEPENDPATH  += $$PWD
INCLUDEPATH += $$PWD



#ffmpeg
unix{
INCLUDEPATH +=/usr/local/ffmpeg/include
LIBS += -L/usr/local/ffmpeg/lib/ -lavfilter -lavformat -lavdevice -lavcodec -lswscale -lavutil -lswresample -lpthread -lm -lz -lrt -ldl
message($$INCLUDEPATH)
message($$LIBS)
}
win32{
INCLUDEPATH += $${OUT_PWD}/ffmpeg4.2/include

LIBS += $${OUT_PWD}/ffmpeg4.2/lib/x86/avcodec.lib\
        $${OUT_PWD}/ffmpeg4.2/lib/x86/avdevice.lib\
        $${OUT_PWD}/ffmpeg4.2/lib/x86/avfilter.lib\
        $${OUT_PWD}/ffmpeg4.2/lib/x86/avformat.lib\
        $${OUT_PWD}/ffmpeg4.2/lib/x86/avutil.lib\
        $${OUT_PWD}/ffmpeg4.2/lib/x86/swresample.lib\
        $${OUT_PWD}/ffmpeg4.2/lib/x86/swscale.lib
}
#ffmpeg的库文件下载到build dir下即可
#https://gitee.com/ch593030323/ffmpeg4.2.git

#~ffmpeg
