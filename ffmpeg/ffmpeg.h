#ifndef FFMPEG_H
#define FFMPEG_H

#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#include "ffmpeghead.h"


struct FFmpegData{
    FFmpegData(){
        init();
    }
    FFmpegData(const QImage &image,
               int cur,
               int total,
               bool isNoError = true,
               const QString &errorString = ""){
        init();
        this->image = image;
        this->cur = cur;
        this->total = total;
        this->isNoError = isNoError;
        this->errorString = errorString;
    }
    FFmpegData(const QString &errorString) {
        init();
        this->isNoError = false;
        this->errorString = errorString;
    }
    inline void init(){
        this->image = QImage();
        this->cur = 0;
        this->total = 0;
        this->isNoError = true;
        this->errorString = "";
    }
    //
    QImage image;
    int cur;    //当前进度
    int total;  //总进度
    bool isNoError;
    QString errorString;
};

struct FFmpegControl{

    void setStopped(bool stopped);
    bool isStopped();

    void setPaused(bool paused);
    bool isPaused();

private:
    bool stopped = false;
    bool paused = false;
    QMutex mutex;
};

class FFmpegObject : public QObject{

    Q_OBJECT
public:
    FFmpegObject(const QString &url, FFmpegControl *mutex, QObject *parent = 0);

public slots:
    void play();

private slots:
    //播放下一帧
    void playNext(bool begin = false);

signals:
    //收到图片信号
    void receiveImage(const FFmpegData &d);

private:
    static void initlib();
    //初始化视频对象
    bool init();
    //释放对象
    void free();

private:
    QString url;                    //视频流地址
    FFmpegControl *mutex;


    int frameFinish;                //一帧完成
    int videoWidth;                 //视频宽度
    int videoHeight;                //视频高度
    int oldWidth;                   //上一次视频宽度
    int oldHeight;                  //上一次视频高度
    int videoStreamIndex;           //视频流索引
    int audioStreamIndex;           //音频流索引


    uint8_t *buffer;                //存储解码后图片buffer
    AVPacket *avPacket;             //包对象
    AVFrame *avFrame;               //帧对象
    AVFrame *avFrame2;              //帧对象
    AVFrame *avFrame3;              //帧对象
    AVFormatContext *avFormatContext;//格式对象
    AVCodecContext *videoCodec;     //视频解码器
    AVCodecContext *audioCodec;     //音频解码器
    SwsContext *swsContext;         //处理图片数据对象

    AVDictionary *options;          //参数对象
    AVCodec *videoDecoder;          //视频解码
    AVCodec *audioDecoder;          //音频解码
};

class FFmpegThread : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegThread(const QString &url, QObject *parent = 0);
    virtual ~FFmpegThread();

    inline QString getUrl() {return url;}
signals:
    //收到图片信号
    void receiveImage(const FFmpegData &d);

    void signalPlay();

protected:
    void run();

public slots:
    void play();
    void pause();
    void stop();

private:
    QString url;           //视频流地址 set only if thread start
    FFmpegControl mutex;
};

#endif // FFMPEG_H
