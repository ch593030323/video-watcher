#include "treevideowidget.h"
#include "playthread.h"

TreeVideoWidget::TreeVideoWidget(QWidget *parent)
    : TreeVideoSearch (parent)
{

}

void TreeVideoWidget::addToPlayThread(const QString &obid, const QString &url)
{
    if(!PlayThread::PlayThreadMap.contains(obid)) {
        PlayThread *thread = new PlayThread(url, this);
        PlayThread::PlayThreadMap.insert(obid,thread);
    }
}
