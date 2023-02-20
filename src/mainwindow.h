#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "videocell.h"
#include "ffmpeg.h"
#include "datasource.h"

#include <QWidget>
#include <QStandardItemModel>


class TreeVideoModel;
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

protected slots:
    void toexit();
    void tovideo_widget();
    void tovideo_playback();
    void tovideo_system_setting();
protected:
    Ui::MainWidget *ui;
};
#endif // MAINWINDOW_H
