#include "treevideoview.h"

#include <QMenu>
#include <QContextMenuEvent>

TreeVideoView::TreeVideoView(QWidget *parent)
    : QTreeView(parent)
    , m_hideMenu(false)
{

}

void TreeVideoView::hideMenu()
{
    m_hideMenu = true;
}

void TreeVideoView::contextMenuEvent(QContextMenuEvent *event)
{
    if(m_hideMenu)
        return;

    QMenu m(this);
    connect(m.addAction(QString::fromUtf8("刷新")),SIGNAL(triggered()),this,SIGNAL(signalRefresh()));
    connect(m.addAction(QString::fromUtf8("折叠全部")),SIGNAL(triggered()),this,SLOT(collapseAll()));
    connect(m.addAction(QString::fromUtf8("导出json文件")),SIGNAL(triggered()),this,SIGNAL(signalExportJson()));
    connect(m.addAction(QString::fromUtf8("导入json文件")),SIGNAL(triggered()),this,SIGNAL(signalImportJson()));
    //connect(m.addAction(QString::fromUtf8("设置")),SIGNAL(triggered()),this,SIGNAL(signalSettings()));
    m.move(this->mapToGlobal(event->pos()));
    m.exec();
}
