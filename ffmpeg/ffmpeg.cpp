#include "ffmpeg.h"

static QByteArray errorToByteArray(int errnum) {
    QByteArray b(AV_ERROR_MAX_STRING_SIZE, '0');
    av_strerror(errnum, b.data(), b.size());
    return b;
}
FFmpegThread::FFmpegThread(const QString &url, QObject *parent)
    : QThread(parent)
    , m_url(url)
    , m_lastPlayState(FFmpegData::NoState)
    , m_tryReconnectIndex(-1)
    , m_tryReconnectMax(3)
{
}

FFmpegThread::~FFmpegThread()
{
    qDebug() << __FUNCTION__ << this->m_url;
}

void FFmpegThread::run()
{
    FFmpegObject obj(m_url, &m_mutex);
    connect(&obj, SIGNAL(receiveImage(FFmpegData)), this, SLOT(slotReceiveImage(FFmpegData)), Qt::QueuedConnection);
    connect(this, SIGNAL(signalPlay()),             &obj, SLOT(play()),                     Qt::QueuedConnection);

    obj.play();
    this->exec();
}

void FFmpegThread::play()
{
    emit signalPlay();
}

void FFmpegThread::pause()
{
    m_mutex.setPaused(true);
}

void FFmpegThread::stop()
{
    m_mutex.setStopped(true);
}

void FFmpegThread::slotReceiveImage(FFmpegData d)
{
    //save state
    if(d.type == FFmpegData::Control) {
        m_lastPlayState = d.playState;
    } else {
        m_lastPlayData = d;
    }
    //reconnect
    if(d.errorCode == FFmpegData::ErrorNetwork) {
        m_tryReconnectIndex ++;
        if(m_tryReconnectIndex < m_tryReconnectMax) {
            d.errorString += QString("\n正在重新连接：%1/%2").arg(m_tryReconnectIndex).arg(m_tryReconnectMax);
            QTimer::singleShot(10, this, SIGNAL(signalPlay()));
        } else {
            m_tryReconnectIndex = -1;
            d.errorString += QString::fromUtf8("\n重新连接失败");
        }
    }
    //send
    emit receiveImage(d);
}

//一个软件中只需要初始化一次就行
FFmpegObject::FFmpegObject(const QString &url, FFmpegControl *mutex, QObject *parent)
    : QObject(parent)
    , mutex(mutex)
    , url(url)
{
    setObjectName("FFmpegObject");

    frameFinish = false;
    videoWidth = 0;
    videoHeight = 0;
    oldWidth = 0;
    oldHeight = 0;
    videoStreamIndex = -1;
    audioStreamIndex = -1;

//    this->url = "rtsp://192.168.1.128:554/1";

    buffer = NULL;
    avPacket = NULL;
    avFrame = NULL;
    avFrame2 = NULL;
    avFrame3 = NULL;
    avFormatContext = NULL;
    videoCodec = NULL;
    audioCodec = NULL;
    swsContext = NULL;

    options = NULL;
    videoDecoder = NULL;
    audioDecoder = NULL;

    process_cur = 0;
    process_cur_org = 0;
    process_total = 0;

    //初始化注册,一个软件中只注册一次即可
    FFmpegObject::initlib();
}

void FFmpegObject::play()
{
    playNext(true);
}

void FFmpegObject::playNext(bool begin)
{
    emit receiveImage(FFmpegData(FFmpegData::Playing));
    process_cur = 0;
    forever
    {
        QImage image;
        process_cur_org = 0;
        process_total = 0;
        //开始播放
        if(begin) {
            begin = false;
            frameFinish = 0;
            process_cur = 0;
            if(!init()) {
                goto end;
            }
        }
        if(mutex->isPaused()) {
            //已经暂停，则本次播放无效
            qDebug() << __FILE__ << __LINE__ << url.section('/', -1) << "手动暂停";
            emit receiveImage(FFmpegData(FFmpegData::Paused));
            goto pause;
        }
        if(mutex->isStopped()) {
            //已经停止，则本次播放结束
            qDebug() << __FILE__ << __LINE__ << url.section('/', -1) << "手动结束";
            emit receiveImage(FFmpegData(FFmpegData::Stopped));
            goto end;
        }

        //清除播放缓存
        if(avPacket) {
            av_packet_unref(avPacket);
            av_freep(avPacket);
        }
        //设置阻塞超时的开始时间
        block_interrupt_data.last_time = time(NULL);
        //读取下一帧
        frameFinish = av_read_frame(avFormatContext, avPacket);
        //播放结束
        if(frameFinish < 0) {
            goto end;
        }
        //获取播放进度
        process_cur_org = avPacket->pts * av_q2d(avFormatContext->streams[avPacket->stream_index]->time_base);
        process_cur = qMax(process_cur, process_cur_org);
        process_total = avFormatContext->duration / AV_TIME_BASE;

        //判断当前包是视频还是音频
        if (avPacket->stream_index == videoStreamIndex) {
            //解码视频流 avcodec_decode_video2 方法已被废弃
#if 1
            avcodec_decode_video2(videoCodec, avFrame2, &frameFinish, avPacket);
#else
            frameFinish = avcodec_send_packet(videoCodec, avPacket);
            if (frameFinish < 0) {
                continue;
            }

            frameFinish = avcodec_receive_frame(videoCodec, avFrame2);
            if (frameFinish < 0) {
                continue;
            }
#endif

            if (frameFinish >= 0) {
                //将数据转成一张图片
                sws_scale(swsContext,
                          (const uint8_t *const *)avFrame2->data,
                          avFrame2->linesize,
                          0,
                          videoHeight,
                          avFrame3->data,
                          avFrame3->linesize);
                //以下两种方法都可以
                //QImage image(avFrame3->data[0], videoWidth, videoHeight, QImage::Format_RGB32);
                image = QImage((uchar *)buffer, videoWidth, videoHeight, QImage::Format_RGB32);

            }
        }
        if (avPacket->stream_index == audioStreamIndex) {
            //解码音频流,这里暂不处理,以后交给sdl播放
        }
        if (!image.isNull()) {
            emit receiveImage(FFmpegData(image, process_cur, process_total));
        }

        av_usleep(10 * 1000);
//        QTimer::singleShot(10, this, SLOT(playNext()));
    }
end:
    //释放资源
    free();
pause:
    //重置标志位
    mutex->setPaused(false);
    mutex->setStopped(false);

    if(frameFinish < 0) {
        qDebug() << __FILE__ << __LINE__<< "block_duration:" << block_interrupt_data.block_duration;
    }

    //播放超时
    if(block_interrupt_data.isTimeOut()) {
        emit receiveImage(FFmpegData(QString::fromUtf8("播放已经断开，请检查网络<read"), FFmpegData::ErrorNetwork));
        return;
    }
    //到文件尾
    if(frameFinish == AVERROR_EOF) {
        playNext(true);
        return;
    }
}

void FFmpegObject::free()
{
    if (swsContext != NULL) {
        sws_freeContext(swsContext);
        swsContext = NULL;
    }

    if (avPacket != NULL) {
        av_packet_unref(avPacket);
        avPacket = NULL;
    }

    if (avFrame != NULL) {
        av_frame_free(&avFrame);
        avFrame = NULL;
    }

    if (avFrame2 != NULL) {
        av_frame_free(&avFrame2);
        avFrame2 = NULL;
    }

    if (avFrame3 != NULL) {
        av_frame_free(&avFrame3);
        avFrame3 = NULL;
    }

    if (videoCodec != NULL) {
        avcodec_close(videoCodec);
        videoCodec = NULL;
    }

    if (audioCodec != NULL) {
        avcodec_close(audioCodec);
        audioCodec = NULL;
    }

    if (avFormatContext != NULL) {
        avformat_close_input(&avFormatContext);
        avFormatContext = NULL;
    }

    av_dict_free(&options);
}

int FFmpegObject::block_interrupt_callback(void *p)
{
    BlockInterruptData *r = (BlockInterruptData *)p;
    if (r->last_time > 0) {
        r->block_duration = time(NULL) - r->last_time;
        if (r->isTimeOut()) {
            //中断
            return 1;
        }
    }
    return 0;
}

void FFmpegObject::initlib()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static bool isInit = false;
    if (!isInit) {
        //注册库中所有可用的文件格式和解码器
        av_register_all();
        //注册所有设备,主要用于本地摄像机播放支持
#ifdef ffmpegdevice
        avdevice_register_all();
#endif
        //初始化网络流格式,使用网络流时必须先执行
        avformat_network_init();

        isInit = true;
        qDebug() << TIMEMS << "init ffmpeg lib ok" << " version:" << FFMPEG_VERSION;
#if 1 //0
        //输出所有支持的解码器名称
        QStringList listCodeName;
        AVCodec *code = av_codec_next(NULL);
        while (code != NULL) {
            listCodeName << code->name;
            code = code->next;
        }

        qDebug() << TIMEMS << listCodeName;
#endif
    }
}

bool FFmpegObject::init()
{
    //资源未释放，无须初始化
    if(avFormatContext)
        return true;
    //在打开码流前指定各种参数比如:探测时间/超时时间/最大延时等
    //设置缓存大小,1080p可将值调大

    //    av_dict_set(&options, "buffer_size", "8192000", 0);
    //以tcp方式打开,如果以udp方式打开将tcp替换为udp
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    //设置超时断开连接时间,单位微秒,3000000表示3秒
    av_dict_set(&options, "stimeout", "3000000", 0);
    //设置最大时延,单位微秒,1000000表示1秒
    av_dict_set(&options, "max_delay", "1000", 0);
    //自动开启线程数
    //    av_dict_set(&options, "threads", "auto", 0);

    //打开视频流
    avFormatContext = avformat_alloc_context();
    //阻塞中断控制
    avFormatContext->interrupt_callback.callback = block_interrupt_callback;
    avFormatContext->interrupt_callback.opaque = &block_interrupt_data;
    block_interrupt_data.last_time = time(NULL);

    int result = avformat_open_input(&avFormatContext, url.toStdString().data(), NULL, &options);//&options
    if (result < 0) {
        //网络超时
        if(AVUNERROR(result) == ETIMEDOUT) {
            emit receiveImage(FFmpegData(QString::fromUtf8("播放已经断开，请检查网络<open"), FFmpegData::ErrorNetwork));
            return false;
        }
        emit receiveImage(FFmpegData(errorToByteArray(result)));
        return false;
    }

    //释放设置参数
    if (options != NULL) {
        av_dict_free(&options);
    }

    //获取流信息
    result = avformat_find_stream_info(avFormatContext, NULL);
    if (result < 0) {
        emit receiveImage(FFmpegData(errorToByteArray(result)));
        qDebug() << TIMEMS << "find stream info error";
        return false;
    }

    //----------视频流部分开始,打个标记方便折叠代码----------
    if (1) {
        videoStreamIndex = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
        if (videoStreamIndex < 0) {
            emit receiveImage(FFmpegData(errorToByteArray(videoStreamIndex)));
            qDebug() << TIMEMS << "find video stream index error" << errorToByteArray(videoStreamIndex);
            return false;
        }

        //获取视频流
        AVStream *videoStream = avFormatContext->streams[videoStreamIndex];

        //获取视频流解码器,或者指定解码器
        videoCodec = videoStream->codec;
        videoDecoder = avcodec_find_decoder(videoCodec->codec_id);
        //videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
        if (videoDecoder == NULL) {
            emit receiveImage(FFmpegData("video decoder not found"));
            qDebug() << TIMEMS << "video decoder not found";
            return false;
        }

        //设置加速解码
        videoCodec->lowres = videoDecoder->max_lowres;
        videoCodec->flags2 |= AV_CODEC_FLAG2_FAST;

        //打开视频解码器
        result = avcodec_open2(videoCodec, videoDecoder, NULL);
        if (result < 0) {
            emit receiveImage(FFmpegData(errorToByteArray(result)));
            qDebug() << TIMEMS << "open video codec error";
            return false;
        }

        //获取分辨率大小
        videoWidth = videoStream->codec->width;
        videoHeight = videoStream->codec->height;

        //如果没有获取到宽高则返回
        if (videoWidth == 0 || videoHeight == 0) {
            qDebug() << TIMEMS << "find width height error";
            emit receiveImage(FFmpegData("find width height error"));
            return false;
        }

        QString videoInfo = QString("视频流信息 -> 索引: %1  解码: %2  格式: %3  时长: %4 秒  分辨率: %5*%6")
                .arg(videoStreamIndex).arg(videoDecoder->name).arg(avFormatContext->iformat->name)
                .arg((avFormatContext->duration) / AV_TIME_BASE).arg(videoWidth).arg(videoHeight);
        qDebug() << TIMEMS << videoInfo;
    }
    //----------视频流部分开始----------

    //----------音频流部分开始,打个标记方便折叠代码----------
    if (1) {
        //循环查找音频流索引
        audioStreamIndex = -1;
        for (uint i = 0; i < avFormatContext->nb_streams; i++) {
            if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStreamIndex = i;
                break;
            }
        }

        //有些没有音频流,所以这里不用返回
        if (audioStreamIndex == -1) {
            qDebug() << TIMEMS << "find audio stream index error";
        } else {
            //获取音频流
            AVStream *audioStream = avFormatContext->streams[audioStreamIndex];
            audioCodec = audioStream->codec;

            //获取音频流解码器,或者指定解码器
            audioDecoder = avcodec_find_decoder(audioCodec->codec_id);
            //audioDecoder = avcodec_find_decoder_by_name("aac");
            if (audioDecoder == NULL) {
                emit receiveImage(FFmpegData("audio codec not found"));
                qDebug() << TIMEMS << "audio codec not found";
                return false;
            }

            //打开音频解码器
            result = avcodec_open2(audioCodec, audioDecoder, NULL);
            if (result < 0) {
                emit receiveImage(FFmpegData(errorToByteArray(result)));
                qDebug() << TIMEMS << "open audio codec error";
                return false;
            }

            QString audioInfo = QString("音频流信息 -> 索引: %1  解码: %2  比特率: %3  声道数: %4  采样: %5")
                    .arg(audioStreamIndex).arg(audioDecoder->name).arg(avFormatContext->bit_rate)
                    .arg(audioCodec->channels).arg(audioCodec->sample_rate);
            qDebug() << TIMEMS << audioInfo;
        }
    }
    //----------音频流部分结束----------

    //预分配好内存
    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
    avFrame2 = av_frame_alloc();
    avFrame3 = av_frame_alloc();

    //比较上一次文件的宽度高度,当改变时,需要重新分配内存
    if (oldWidth != videoWidth || oldHeight != videoHeight) {
        int byte = avpicture_get_size(AV_PIX_FMT_RGB32, videoWidth, videoHeight);
        buffer = (uint8_t *)av_malloc(byte * sizeof(uint8_t));
        oldWidth = videoWidth;
        oldHeight = videoHeight;
    }

    //定义像素格式
    AVPixelFormat srcFormat = AV_PIX_FMT_YUV420P;
    AVPixelFormat dstFormat = AV_PIX_FMT_RGB32;
    //通过解码器获取解码格式
    srcFormat = videoCodec->pix_fmt;

    //默认最快速度的解码采用的SWS_FAST_BILINEAR参数,可能会丢失部分图片数据,可以自行更改成其他参数
    int flags = SWS_FAST_BILINEAR;

    //开辟缓存存储一帧数据
    //以下两种方法都可以,avpicture_fill已经逐渐被废弃
    //avpicture_fill((AVPicture *)avFrame3, buffer, dstFormat, videoWidth, videoHeight);
    av_image_fill_arrays(avFrame3->data, avFrame3->linesize, buffer, dstFormat, videoWidth, videoHeight, 1);

    //图像转换
    swsContext = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, dstFormat, flags, NULL, NULL, NULL);

    //输出视频信息
    //av_dump_format(avFormatContext, 0, url.toStdString().data(), 0);

    //qDebug() << TIMEMS << "init ffmpeg finsh";
    return true;
}

void FFmpegControl::setStopped(bool stopped)
{
    QMutexLocker locker(&mutex);
    this->stopped = stopped;
}

bool FFmpegControl::isStopped()
{
    QMutexLocker locker(&mutex);
    return stopped;
}

void FFmpegControl::setPaused(bool paused)
{
    QMutexLocker locker(&mutex);
    this->paused = paused;
}

bool FFmpegControl::isPaused()
{
    QMutexLocker locker(&mutex);
    return paused;
}

FFmpegObject::BlockInterruptData::BlockInterruptData()
    : last_time(0)
    , block_duration(0)
    , max_block_duration(8)
{

}

bool FFmpegObject::BlockInterruptData::isTimeOut()
{
    return block_duration > max_block_duration;
}
