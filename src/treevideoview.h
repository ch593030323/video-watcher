#ifndef TREEVIDEOVIEW_H
#define TREEVIDEOVIEW_H

#include <QTreeView>

/**
 * @brief The TreeVideoTransfer class
 * 信号转发用
 */
class TreeVideoSignalTransfer : public QObject {

    Q_OBJECT
public:
    TreeVideoSignalTransfer(QObject *parent = 0);

    void triggerUpdateUrl();
signals:
    void signalUpdateUrl();
};

class TreeVideoView : public QTreeView
{
    Q_OBJECT
public:
    TreeVideoView(QWidget *parent = 0);

    void hideMenu();

public slots:
    void slotExpandAll();
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
