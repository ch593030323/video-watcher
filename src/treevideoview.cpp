#include "treevideoview.h"
#include "treevideomodel.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QStandardItem>
#include <QScrollBar>

TreeVideoView::TreeVideoView(QWidget *parent)
    : QTreeView(parent)
    , m_hideMenu(false)
{
    setFocusPolicy(Qt::NoFocus);
    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEmitUrl(QModelIndex)));
}

void TreeVideoView::hideMenu()
{
    m_hideMenu = true;
}

void TreeVideoView::slotExpandAll()
{
	//直接调用expandAll会有崩溃的问题，故用新的方法
    QStandardItemModel *m_treeModel = qobject_cast<QStandardItemModel *>(this->model());
    QStandardItem *itemRoot     = m_treeModel->invisibleRootItem();
    QStandardItem *itemISCS     = itemRoot ? itemRoot->child(0) : 0;
    if(!itemISCS)
        return;
    this->expand(itemISCS->index());
    for(int k = 0; k < itemISCS->rowCount(); k ++) {
        QStandardItem *itemStation  = itemISCS->child(k);
        if(!itemStation)
            continue;
        this->expand(itemStation->index());
    }
}

void TreeVideoView::slotEmitUrl(const QModelIndex &index)
{
    emit signalPlayRequest(index.data(VideoUrlRole).toString());
}

void TreeVideoView::contextMenuEvent(QContextMenuEvent *event)
{
    if(m_hideMenu)
        return;

    QMenu m(this);
    connect(m.addAction(QString::fromUtf8("刷新")),SIGNAL(triggered()),this,SIGNAL(signalRefresh()));
    connect(m.addAction(QString::fromUtf8("全部展开")),SIGNAL(triggered()),this,SLOT(slotExpandAll()));
    connect(m.addAction(QString::fromUtf8("折叠全部")),SIGNAL(triggered()),this,SLOT(collapseAll()));
    //connect(m.addAction(QString::fromUtf8("导出json文件")),SIGNAL(triggered()),this,SIGNAL(signalExportJson()));
    //connect(m.addAction(QString::fromUtf8("导入json文件")),SIGNAL(triggered()),this,SIGNAL(signalImportJson()));
//    connect(m.addAction(QString::fromUtf8("设置")),SIGNAL(triggered()),this,SIGNAL(signalSettings()));
    m.move(this->mapToGlobal(event->pos()));
    m.exec();
}

TreeVideoSignalTransfer::TreeVideoSignalTransfer(QObject *parent)
    : QObject(parent)
{

}

void TreeVideoSignalTransfer::triggerUpdateUrl()
{
    emit signalUpdateUrl();
}
