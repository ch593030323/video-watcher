#include "playthread.h"

QMap<QString, PlayThread *> PlayThread::PlayThreadMap;
QObject *PlayThread::threadParent = 0;

PlayThread *PlayThread::createPlayThread(const QString &url)
{
    //确保父类唯一
    Q_ASSERT(threadParent);

    return new PlayThread(url, threadParent);
}

PlayThread::PlayThread(const QString &url, QObject *parent)
    : FFmpegThread(url, parent)
{

    qRegisterMetaType<FFmpegData>("FFmpegData");
}

PlayThread::~PlayThread()
{
    if (this->isRunning()) {
        this->stop();
        this->quit();
        this->wait();
    }
}

int PlayThread::receiverImageConnectionCount() const
{
    return QObject::receivers(SIGNAL(receiveImage(FFmpegData)));
}

void PlayThread::open()
{
    if(isRunning()) {
        FFmpegThread::play();
    } else {
        FFmpegThread::start();
    }
}

void PlayThread::close()
{
    this->stop();
}
