#include "playthread.h"
#include "lds.h"

QMap<QString, PlayThread *> PlayThread::PlayThreadMap;
QObject *PlayThread::threadParent = 0;

PlayThread *PlayThread::createPlayThread(const QString &url)
{
    //确保父类唯一
    Q_ASSERT(threadParent);

    return new PlayThread(url, threadParent);
}

void PlayThread::snap()
{
    if(m_lastPlayData.image.isNull()) {
        lds::showMessage(tr("保存失败"), tr("图片为空"));
        return;
    }

    QString dir = lds::configDirectory + "/snap";
    QDir().mkpath(dir);
    QString filepath = lds::getUniqueFilePathhByDateTime(dir, "", "png");

    QImageWriter writer(filepath);
    writer.setText("url", m_url);
    if(!writer.write(m_lastPlayData.image)) {
        lds::showMessage(tr("保存失败"), writer.errorString());
        return;
    }

    lds::showMessage(tr("保存成功"), filepath);
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
//        FFmpegThread::play();
    } else {
        FFmpegThread::start();
    }
}

void PlayThread::close()
{
    this->stop();
}
