#include "treevideoviewsearchurl.h"
#include "playthread.h"

#include <QtDebug>

TreeVideoViewSearchUrl::TreeVideoViewSearchUrl(QWidget *parent)
    : TreeVideoViewSearch (parent)
{

}

void TreeVideoViewSearchUrl::addToPlayThread(const QString &url)
{
    if(!url.isEmpty() && !PlayThread::PlayThreadMap.contains(url)) {
        PlayThread *thread = PlayThread::createPlayThread(url);
        PlayThread::PlayThreadMap.insert(url, thread);
    }
}
