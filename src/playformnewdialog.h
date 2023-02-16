#ifndef PLAYFORMNEWDIALOG_H
#define PLAYFORMNEWDIALOG_H

#include "lds.h"
#include "videolayout.h"
#include "datasource.h"

#include <QDialog>
#include <QMap>

class VideoCell;

namespace Ui {
class playformnewdialog;
}

class playformnewdialog : public QDialog
{
    Q_OBJECT

public:
    explicit playformnewdialog(QWidget *parent = 0);
    ~playformnewdialog();

    void setDataSource(DataSource *datasource);

    void updateLayout(bool isRemovePlayer = false);

protected:
    void contextMenuEvent(QContextMenuEvent * event);

    void resizeEvent(QResizeEvent *event);

private:
    QList<VideoCell *> selectedWidgetList();

private slots:
    void toVideoLayout1x1();
    void toVideoLayout2x2();
    void toVideoLayout3x3();
    void toVideoLayout4x4();
    void toVideoLayout5x5();

    void toaccept();
    void toexit();
    void tomerge();
    void toclear();
    void torestore();

    void toselected();
    void tounselectedall();
private:
    Ui::playformnewdialog               *ui;
    QMap<LayoutPos, VideoCell *>      m_videoMap;
    LayoutInfo                          m_layoutInfo;
};

#endif // PLAYFORMNEWDIALOG_H
