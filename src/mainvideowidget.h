#ifndef MAINVIDEOWIDGET_H
#define MAINVIDEOWIDGET_H

#include "videocell.h"
#include "ffmpeg.h"
#include "datasource.h"

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class MainVideoWidget;
}

class MainVideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainVideoWidget(QWidget *parent = 0);
    ~MainVideoWidget();


    void updateLayout();
    void setDataSource(DataSource *datasource);

    void saveLayout();
public slots:
    void loadLayout();
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

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::MainVideoWidget *ui;
};

#endif // MAINVIDEOWIDGET_H
