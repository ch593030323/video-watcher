#ifndef FFMPEG_H
#define FFMPEG_H

#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#include "ffmpeghead.h"


class FFmpegData{
public:
    enum ErrorCode{
        NoError,
        ErrorNetwork,
        ErrorOther
    };
    enum Type{
        Image,
        Control
    };
    enum PlayState{
        NoState,
        Playing,
        Paused,
        Stopped
    };

    //0
    FFmpegData(){
        init();
    }
    //3、4、5
    FFmpegData(const QImage &image,
               int cur,
               int total,
               ErrorCode errorCode = NoError,
               const QString &errorString = "") {
        init();
        this->image = image;
        this->cur = cur;
        this->total = total;
        this->errorCode = errorCode;
        this->errorString = errorString;
    }

    //1、2
    FFmpegData(const QString &errorString,
               ErrorCode errorCode = ErrorOther) {
        init();
        this->errorCode = errorCode;
        this->errorString = errorString;
    }

    //1 控制命令
    FFmpegData(PlayState state) {
        init();
        this->type = Control;
        this->playState = state;
    }

    inline void init(){
        this->image = QImage();
        this->cur = 0;
        this->total = 0;
        this->errorCode = NoError;
        this->errorString = "";
        this->type = Image;
        this->playState = NoState;
    }
    //
    QImage image;
    int cur;    //当前进度
    int total;  //总进度
    ErrorCode errorCode;
    QString errorString;
    Type type;
    PlayState playState;
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

    //播放进度
    int process_cur;        //当前进度，单位秒
    int process_cur_org;    //当前进度,单位秒，可能小于process_cur
    int process_total;      //视频总时长，单位秒

    /**
     * @brief The BlockInterruptData struct
     * 中断阻塞的回调结构体
     */
    struct BlockInterruptData {
        BlockInterruptData();

        time_t last_time;//阻塞前的时间，单位s
        int block_duration;//阻塞的时长，单位s
        const int max_block_duration;//最长阻塞时间，单位s

        bool isTimeOut();
    };

    // 回调函数
    static int block_interrupt_callback(void *p);
    BlockInterruptData block_interrupt_data;
};

class FFmpegThread : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegThread(const QString &url, QObject *parent = 0);
    virtual ~FFmpegThread();

    inline QString getUrl() {return m_url;}
    inline FFmpegData::PlayState lastPlayState() {return m_lastPlayState;}
    inline FFmpegData lastPlayData() {return m_lastPlayData;}
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

private slots:
    void slotReceiveImage(FFmpegData d);

private:
    QString m_url;           //视频流地址 set only if thread start
    FFmpegControl m_mutex;

    FFmpegData::PlayState m_lastPlayState;
    FFmpegData m_lastPlayData;

    int m_tryReconnectIndex;
    const int m_tryReconnectMax;
};

#endif // FFMPEG_H
