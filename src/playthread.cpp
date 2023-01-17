#include "playthread.h"

QMap<QString, PlayThread *> PlayThread::PlayThreadMap;

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
