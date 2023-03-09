#include "mainsystemsetingssnaplistview.h"

#include <QMenu>
#include <QContextMenuEvent>

MainSystemSetingsSnapListView::MainSystemSetingsSnapListView(QWidget *parent)
    : QListView(parent)
{

}

void MainSystemSetingsSnapListView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu m(this);
    connect(m.addAction(QString::fromUtf8("删除")),SIGNAL(triggered()),this,SIGNAL(signalDelete()));
    connect(m.addAction(QString::fromUtf8("刷新")),SIGNAL(triggered()),this,SIGNAL(signalRefresh()));
    m.move(this->mapToGlobal(event->pos()));
    m.exec();
}
