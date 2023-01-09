#include "treevideoview.h"

#include <QMenu>
#include <QContextMenuEvent>

TreeVideoView::TreeVideoView(QWidget *parent) : QTreeView(parent)
{

}

void TreeVideoView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu m(this);
    connect(m.addAction(QString::fromUtf8("刷新")),SIGNAL(triggered()),this,SIGNAL(signalRefresh()));
    connect(m.addAction(QString::fromUtf8("折叠全部")),SIGNAL(triggered()),this,SLOT(collapseAll()));
    //connect(m.addAction(QString::fromUtf8("设置")),SIGNAL(triggered()),this,SIGNAL(signalSettings()));
    m.move(this->mapToGlobal(event->pos()));
    m.exec();
}
