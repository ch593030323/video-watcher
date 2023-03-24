#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QString url,QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);this->setWindowTitle(lds::windowName);
    setWindowFlags(Qt::Window);

    QStringList urls;
    urls << "https://hls01open.ys7.com/openlive/6e0b2be040a943489ef0b9bb344b96b8.hd.m3u8";
    urls << "rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";
    urls << "http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4";
    urls << "rtsp://admin:Admin123456@192.168.0.15:554/media/video1";
    urls << "rtsp://10.137.32.250:554/rtp/34020000001320000001_34020000001310000001";
    urls << "rtsp://admin:*Dt12a34b@10.137.32.110";
    urls << "rtsp://127.0.0.1:8554/22";

    m_url = url;
    urls << url;
    ui->cboxUrl->addItems(urls);
    ui->cboxUrl->setCurrentIndex(urls.length() - 1);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == QString::fromUtf8("打开")) {
        ui->btnOpen->setText(QString::fromUtf8("关闭"));
        QString url = ui->cboxUrl->currentText().trimmed();
        ui->playWidget->setUrl(url);
        ui->playWidget->open();
    } else {
        ui->btnOpen->setText(QString::fromUtf8("打开"));
        ui->playWidget->close();
    }
}
