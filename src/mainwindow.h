#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videocell.h"
#include "ffmpeg.h"
#include "dataSource.h"

#include <QWidget>
#include <QStandardItemModel>


class TreeVideoModel;
class MainVideoWidget;
class MainSystemSetings;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

//这里用MainWindow的话，插件编译会有问题，做记录
class MainFrame : public QFrame
{
    Q_OBJECT
public:
    MainFrame(QWidget *parent = 0);
    ~MainFrame();

    void setDataSource(DataSource *dataSource);

    void showMessage(const QString &text);

    bool isVisiableStatusBar();

public slots:
    void setVisiableStatusBar(bool showed);

    void saveLayout();
    void loadLayout();

protected slots:
    void toexit();
    void tovideo_widget();
    void tovideo_playback();
    void tovideo_system_setting();
protected:
    Ui::MainWidget *ui;

    DataSource *m_dataSource;
    MainVideoWidget *m_videoWidget;
    MainSystemSetings *m_systemSettings;
};
#endif // MAINWINDOW_H
