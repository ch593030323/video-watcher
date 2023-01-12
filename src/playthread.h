#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include "ffmpeg.h"

/**
 * @brief The PlayThread class
 * 在原有播放线程的基础上加了些优化函数
 */
class PlayThread : public FFmpegThread{
    Q_OBJECT
public:
    explicit PlayThread(const QString &url, QObject *parent = 0) : FFmpegThread(url, parent) {}
    virtual ~PlayThread();
    int receiverImageConnectionCount() const;
    void open();
    void close();
};

#endif // PLAYTHREAD_H
