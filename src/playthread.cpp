#include "playthread.h"

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
