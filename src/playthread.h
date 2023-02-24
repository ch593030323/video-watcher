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
    //url and play thread
    static QMap<QString, PlayThread *> PlayThreadMap;
    //统一父类，便于统一释放资源，只赋值一次
    static QObject *threadParent;
    static PlayThread *createPlayThread(const QString &url);

private:
    explicit PlayThread(const QString &url, QObject *parent = 0);
    virtual ~PlayThread();
public:
    int receiverImageConnectionCount() const;
    void open();
    void close();
};

#endif // PLAYTHREAD_H
