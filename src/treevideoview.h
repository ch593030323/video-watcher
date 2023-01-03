#ifndef TREEVIDEOVIEW_H
#define TREEVIDEOVIEW_H

#include <QTreeView>

class TreeVideoView : public QTreeView
{
    Q_OBJECT
public:
    TreeVideoView(QWidget *parent = 0);

signals:
    void signalRefresh();

protected:
    void contextMenuEvent(QContextMenuEvent * event);
};

#endif // TREEVIDEOVIEW_H
