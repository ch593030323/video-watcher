#ifndef TREEVIDEOVIEW_H
#define TREEVIDEOVIEW_H

#include <QTreeView>

class TreeVideoView : public QTreeView
{
    Q_OBJECT
public:
    TreeVideoView(QWidget *parent = 0);

    void hideMenu();
private slots:
    void toExpand();
signals:
    void signalRefresh();
    void signalSettings();
    void signalExportJson();
    void signalImportJson();

protected:
    void contextMenuEvent(QContextMenuEvent * event);

private:
    bool m_hideMenu;
};

#endif // TREEVIDEOVIEW_H
