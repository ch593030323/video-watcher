#include "playformnewdialog.h"
#include "ui_playformnewdialog.h"
#include "videocell.h"
#include "lds.h"
#include "treevideoview.h"

#include <QStandardItemModel>
#include <QApplication>
#include <QDesktopWidget>

playformnewdialog::playformnewdialog(Type type, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::playformnewdialog)
    , m_type(type)
{
    ui->setupUi(this);
    setObjectName("Window");
    resize(832, 557);

    ui->widget_video->setMultilselected(true);
    ui->pushButton_ok->setText(type == TypeNew ? QString::fromUtf8("新增") : QString::fromUtf8("修改"));
    if(type == TypeNew) {
        ui->lineEdit->setText(lds::getUniqueFileNamehByDateTime(lds::configDirectory + "/play_form"));
    }
    if(type == TypeModify) {
        ui->lineEdit->setEnabled(false);
    }

    //
    ui->pushButton_1x1->setIcon(lds::getLayoutPixmap(1));
    ui->pushButton_1x1->setText("");
    ui->pushButton_2x2->setIcon(lds::getLayoutPixmap(2));
    ui->pushButton_2x2->setText("");
    ui->pushButton_3x3->setIcon(lds::getLayoutPixmap(3));
    ui->pushButton_3x3->setText("");
    ui->pushButton_4x4->setIcon(lds::getLayoutPixmap(4));
    ui->pushButton_4x4->setText("");
    ui->pushButton_5x5->setIcon(lds::getLayoutPixmap(5));
    ui->pushButton_5x5->setText("");

    //buttongroup
    ui->pushButton_1x1->setCheckable(true);
    ui->pushButton_2x2->setCheckable(true);
    ui->pushButton_3x3->setCheckable(true);
    ui->pushButton_4x4->setCheckable(true);
    ui->pushButton_5x5->setCheckable(true);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_1x1);
    buttonGroup->addButton(ui->pushButton_2x2);
    buttonGroup->addButton(ui->pushButton_3x3);
    buttonGroup->addButton(ui->pushButton_4x4);
    buttonGroup->addButton(ui->pushButton_5x5);
    buttonGroup->setExclusive(true);

    //
    ui->pushButton_ok->setIcon(lds::getFontPixmap(0x2713));
    ui->pushButton_exit->setIcon(lds::getFontPixmap(0xd7));
    //connect
    connect(ui->pushButton_1x1, SIGNAL(clicked()), this, SLOT(toVideoLayout1x1()));
    connect(ui->pushButton_2x2, SIGNAL(clicked()), this, SLOT(toVideoLayout2x2()));
    connect(ui->pushButton_3x3, SIGNAL(clicked()), this, SLOT(toVideoLayout3x3()));
    connect(ui->pushButton_4x4, SIGNAL(clicked()), this, SLOT(toVideoLayout4x4()));
    connect(ui->pushButton_5x5, SIGNAL(clicked()), this, SLOT(toVideoLayout5x5()));

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(toaccept()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->treeView->view(), SIGNAL(doubleClicked(QModelIndex)), ui->widget_video, SLOT(slotAutoAddUrl(QModelIndex)));

    toVideoLayout3x3();
    ui->pushButton_3x3->setChecked(true);
}

playformnewdialog::~playformnewdialog()
{
    delete ui;
}

void playformnewdialog::setDataSource(DataSource *datasource)
{
    ui->treeView->setDataSource(datasource);
    ui->treeView->slotInitTree();
}

void playformnewdialog::updateLayout()
{
    ui->widget_video->updateLayout();
}

void playformnewdialog::readFrom(const QString &filepath)
{
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        lds::showMessage((file.errorString() + ":" + file.fileName()));
    }
    QByteArray readall = file.readAll();

    ui->widget_video->updateLayout(LayoutInfo::readFrom(readall));
    ui->lineEdit->setText(QFileInfo(filepath).baseName());

    //checked
    if(1 == ui->widget_video->columnCount()) {
        ui->pushButton_1x1->setChecked(true);
    } else if(2 == ui->widget_video->columnCount()) {
        ui->pushButton_2x2->setChecked(true);
    } else if(3 == ui->widget_video->columnCount()) {
        ui->pushButton_3x3->setChecked(true);
    } else if(4 == ui->widget_video->columnCount()) {
        ui->pushButton_4x4->setChecked(true);
    } else if(5 == ui->widget_video->columnCount()) {
        ui->pushButton_5x5->setChecked(true);
    }
}

void playformnewdialog::resizeEvent(QResizeEvent *event)
{
    ui->widget_video->updateLayout();
}

void playformnewdialog::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget::contextMenuEvent(event);
}

void playformnewdialog::toVideoLayout1x1()
{
   ui->widget_video->updateLayout(LayoutInfo(1));
}

void playformnewdialog::toVideoLayout2x2()
{
    ui->widget_video->updateLayout(LayoutInfo(2));
}

void playformnewdialog::toVideoLayout3x3()
{
    ui->widget_video->updateLayout(LayoutInfo(3));
}

void playformnewdialog::toVideoLayout4x4()
{
    ui->widget_video->updateLayout(LayoutInfo(4));
}

void playformnewdialog::toVideoLayout5x5()
{
    ui->widget_video->updateLayout(LayoutInfo(5));
}

void playformnewdialog::toaccept()
{
    QDir().mkpath(lds::configDirectory + "/play_form");
    QFile file(lds::configDirectory + QString("/play_form/%1.txt").arg(ui->lineEdit->text().trimmed()));
    if(!file.open(QFile::WriteOnly)) {
        lds::showMessage((file.errorString() + ":" + file.fileName()));
    }
    file.write(ui->widget_video->toJson());

    this->accept();
}

void playformnewdialog::toexit()
{
    this->reject();
}

