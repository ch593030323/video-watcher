#include "treevideoviewsearchurl.h"
#include "playthread.h"

#include <QtDebug>

TreeVideoViewSearchUrl::TreeVideoViewSearchUrl(QWidget *parent)
    : TreeVideoViewSearch (parent)
{

}

void TreeVideoViewSearchUrl::addToPlayThread(const QString &obid, const QString &url)
{
    if(!PlayThread::PlayThreadMap.contains(obid) && !url.isEmpty()) {
        PlayThread *thread = new PlayThread(url, this);
        PlayThread::PlayThreadMap.insert(obid, thread);
    }
}
