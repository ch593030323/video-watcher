#include "treevideoviewsearchurl.h"
#include "playthread.h"

TreeVideoViewSearchUrl::TreeVideoViewSearchUrl(QWidget *parent)
    : TreeVideoViewSearch (parent)
{

}

void TreeVideoViewSearchUrl::addToPlayThread(const QString &obid, const QString &url)
{
    if(!PlayThread::PlayThreadMap.contains(obid)) {
        PlayThread *thread = new PlayThread(url, this);
        PlayThread::PlayThreadMap.insert(obid,thread);
    }
}
