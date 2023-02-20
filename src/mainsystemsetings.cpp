#include "mainsystemsetings.h"
#include "ui_mainsystemsetings.h"
#include "json/json.h"
#include "treevideomodel.h"
#include "playformnewdialog.h"
#include "playalternatenewdialog.h"
#include "lds.h"
#include "treevideoview.h"
#include "propertycolor.h"

#include <QTreeView>
#include <QStandardItemModel>
#include <QtDebug>
#include <QWidgetAction>
#include <QMenu>
#include <QDir>
#include <QDirIterator>

MainSystemSetings::MainSystemSetings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainSystemSetings)
{
    ui->setupUi(this);

    ui->pushButton_play_alternate->setCheckable(true);
    ui->pushButton_play_form->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->pushButton_play_alternate);
    buttonGroup->addButton(ui->pushButton_play_form);
    buttonGroup->setExclusive(true);

    ui->pushButton_play_form->setChecked(true);
    toform();


    connect(ui->pushButton_play_alternate, SIGNAL(clicked()),this,SLOT(toalter()));
    connect(ui->pushButton_play_form, SIGNAL(clicked()),this,SLOT(toform()));



//    ui->frame_play_alternate->setObjectName("Window");
//    ui->frame_play_form->setObjectName("Window");

//    //
//    {
//        ui->frame_play_form->setFixedSize(500, 60);
//        ui->frame_play_form->layout()->setMargin(10);
//        QWidgetAction *wac = new QWidgetAction( ui->pushButton_play_form);
//        wac->setDefaultWidget(ui->frame_play_form);
//        QMenu *menu_form = new QMenu(ui->pushButton_play_form);
//        menu_form->addAction(wac);
//        ui->pushButton_play_form->setMenu(menu_form);
//    }
//    {
//        ui->frame_play_alternate->setFixedSize(500, 60);
//        ui->frame_play_alternate->layout()->setMargin(10);
//        QWidgetAction *wac = new QWidgetAction( ui->pushButton_play_alternate);
//        wac->setDefaultWidget(ui->frame_play_alternate);
//        QMenu *menu_alternate = new QMenu(ui->pushButton_play_alternate);
//        menu_alternate->addAction(wac);
//        ui->pushButton_play_alternate->setMenu(menu_alternate);
//    }

//    //play_form
//    {
//        QDirIterator dir("play_form");
//        while(dir.hasNext()) {
//            dir.next();
//            if(dir.fileInfo().isFile()) {
//                ui->comboBox_form->addItem(dir.fileName(), dir.filePath());
//            }
//        }
//    }

//    //play_alter
//    {
//        QDirIterator dir("play_alter");
//        while(dir.hasNext()) {
//            dir.next();
//            if(dir.fileInfo().isFile()) {
//                ui->comboBox_alternate->addItem(dir.fileName(), dir.filePath());
//            }
//        }
//    }


//    connect(ui->pushButton_form_new, SIGNAL(clicked()), this, SLOT(toPlayFormNew()));
//    connect(ui->pushButton_form_ok, SIGNAL(clicked()), this, SLOT(toPlayFormOK()));
//    connect(ui->pushButton_alternate_new, SIGNAL(clicked()), this, SLOT(toPlayAlternateNew()));
//    connect(ui->pushButton_alternate_ok, SIGNAL(clicked()), this, SLOT(toPlayAlternateOK()));
//    connect(ui->pushButton_play_alternate->menu(), SIGNAL(aboutToShow()), this, SLOT(toHoldVideoFocus()));
//    connect(ui->pushButton_play_alternate->menu(), SIGNAL(aboutToHide()), this, SLOT(toReleaseVideoFocus()));

}

MainSystemSetings::~MainSystemSetings()
{
    delete ui;
}

void MainSystemSetings::toPlayFormNew()
{
    playformnewdialog dialog(this);
    //dialog.setDataSource(ui->treeView->dataSource());
    dialog.resize(this->size());
    dialog.exec();
}

void MainSystemSetings::toPlayFormOK()
{
//    int row = ui->comboBox_form->currentIndex();
//    if(row < 0)
//        return;
//    QFile file(ui->comboBox_form->itemData(row).toString());
//    if(!file.open(QFile::ReadOnly)) {
//        qDebug() << file.errorString();
//    }
//    LayoutInfo info =  LayoutInfo::readFrom(file.readAll());
    //ui->widget_video->updateLayout(info);
}

void MainSystemSetings::toPlayAlternateNew()
{
    PlayAlternateNewDialog dialog(this);
    //dialog.setDataSource(ui->treeView->dataSource());
    dialog.resize(this->size());
    dialog.exec();
}

void MainSystemSetings::toPlayAlternateOK()
{
//    if(!VideoCell::lastFocusWidget)
//        return;
//    int row = ui->comboBox_alternate->currentIndex();
//    if(row < 0)
//        return;

    //    VideoCell::lastFocusWidget->addPlayerList(AlterPlayFrame::readFrom(ui->comboBox_alternate->itemData(row).toString()));
}

void MainSystemSetings::toalter()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainSystemSetings::toform()
{
    ui->stackedWidget->setCurrentIndex(1);
}
