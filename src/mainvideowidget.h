#ifndef MAINVIDEOWIDGET_H
#define MAINVIDEOWIDGET_H

#include "videocell.h"
#include "ffmpeg.h"
#include "datasource.h"

#include <QWidget>
#include <QStandardItemModel>

class Ui_MainVideoWidget;
class MainVideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainVideoWidget(QWidget *parent = 0);
    ~MainVideoWidget();

    void updateLayout();
    void setDataSource(DataSource *datasource);

public:
    void updateTreeVisiableItemUrl();

signals:
    void signalStatusbarSetVisiable(bool showed);

private slots:

    /**
     * @brief toVideoLayout1x1
     * 三种video的布局
     */
    void toVideoLayout1x1();
    void toVideoLayout2x2();
    void toVideoLayout3x3();
    void toVideoLayout4x4();

    void toVideoShowMax();
    void toVideoShowNormal();

    void toUpdateMoreMenu();
    void toApplyLayoutInfo();
    void toNewFormPlay();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui_MainVideoWidget *ui;
    friend class MainFrame;
};

#endif // MAINVIDEOWIDGET_H
