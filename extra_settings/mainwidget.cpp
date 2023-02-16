#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "json/json.h"
#include "src/treevideoviewsearchurl.h"
#include "src/treevideomodel.h"
#include "src/treevideoview.h"

#include <QFile>
#include <QSqlQuery>
#include <QtDebug>
#include <QTreeView>
#include <QItemSelectionModel>
#include <QButtonGroup>


mainWidget::mainWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    this->setObjectName("Window");
    ui->lineEdit_default_rowcount->enableOnlyNumberInput();
    ui->lineEdit_default_columncount->enableOnlyNumberInput();

    conf = new QSettings("setting.ini", QSettings::IniFormat, this);
    ui->lineEdit_default_rowcount->setText(conf->value("global/default_rowcount", 2).toString());
    ui->lineEdit_default_columncount->setText(conf->value("global/default_columncount", 2).toString());
    ui->tableView_defatul_playlist->setStringList(conf->value("global/default_playlist", QStringList()
                                                          <<"http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4"
                                                          <<"http://vfx.mtime.cn/Video/2021/01/07/mp4/210107172407759182_1080.mp4"
                                                          <<"http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4"
                                                          <<"http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp")
                                                  .toStringList());

    m_datasource = new JsonDataSource("D:/Users/Dy/Documents/qt_project/build-VideoWatcher-Desktop_Qt_5_9_6_MinGW_32bit-Debug/video.json", this);
    //ui->treeView->hideMenu();
    ui->treeView->setDataSource(m_datasource);

    //buttongroup
    ui->toolButton_basic->setCheckable(true);
    ui->toolButton_camera->setCheckable(true);
    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->toolButton_basic);
    buttonGroup->addButton(ui->toolButton_camera);
    buttonGroup->setExclusive(true);
    ui->toolButton_basic->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);


    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(buttonGroup, SIGNAL(buttonToggled(QAbstractButton *, bool)), this, SLOT(toSwitchPage(QAbstractButton *, bool)));
    connect(ui->pushButton_add_defatult_video_url, SIGNAL(clicked()), ui->tableView_defatul_playlist, SLOT(toAdd()));
    connect(ui->pushButton_sub_defatult_video_url, SIGNAL(clicked()), ui->tableView_defatul_playlist, SLOT(toRemove()));

    toreload();
}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::toexit()
{
    this->close();
}

void mainWidget::toreload()
{
    if(!m_datasource->update())
        return;
    ui->treeView->slotInitAll();
}

void mainWidget::toSwitchPage(QAbstractButton *button, bool checked)
{
    if(!checked)
        return;
    if(button == ui->toolButton_basic) {
        ui->stackedWidget->setCurrentIndex(0);
    }
    if(button == ui->toolButton_camera) {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void mainWidget::tosave()
{
    conf->setValue("global/default_rowcount", ui->lineEdit_default_rowcount->text());
    conf->setValue("global/default_columncount", ui->lineEdit_default_columncount->text());
    conf->setValue("global/default_playlist", ui->tableView_defatul_playlist->result());
}

JsonDataSource::JsonDataSource(const QString &jsonPath, QObject *parent)
    : DataSource(parent)
{
    m_jsonPath = jsonPath;
}

bool JsonDataSource::update()
{
    QSqlQuery query;
    QFile file(m_jsonPath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return false;
    }
    QByteArray json =  file.readAll();
    Json::Value value;
    Json::Reader reader;
    if(!reader.parse(json.begin(), json.end(), value)) {
        qDebug() << "json formate is error";
    }
    Json::Value global = value["global"];
    query.exec(QString("replace into vw_settings values('settings:default_column_count', '%1')")
               .arg(global["settings"]["default_column_count"].asInt()));
    query.exec(QString("replace into vw_settings values('settings:default_row_count', '%1')")
               .arg(global["settings"]["default_row_count"].asInt()));
    query.exec(QString("replace into vw_settings values('settings:default_video_url_list', '%1')")
               .arg(global["settings"]["default_video_url_list"].asCString()));

    m_locationList.clear();
    m_cameraList.clear();
    m_cameraStateList.clear();

    Json::Value state_choice = value["global"]["state_choice"];
    for(int k = 0; k < state_choice.size(); k++) {
        CameraState item;
        item.rank = state_choice[k]["rank"].asInt();
        item.name = state_choice[k]["name"].asCString();
        m_cameraStateList << item;
    }

    Json::Value station = value["station"];
    for(int k1 = 0; k1 < station.size(); k1 ++) {
        Json::Value cameraArray = station[k1]["camera"];
        QString location_obid = station[k1]["obid"].asCString();
        QString location_name = station[k1]["name"].asCString();

        Location location_item;
        location_item.obid = location_obid;
        location_item.name = location_name;
        location_item.state = 0;
        location_item.camera_count = cameraArray.size();
        m_locationList << location_item;

        for(int k2 = 0; k2 < cameraArray.size(); k2 ++) {
            Camera camera_item;
            camera_item.location_obid = location_obid;
            camera_item.obid = cameraArray[k2]["obid"].asCString();
            camera_item.name = cameraArray[k2]["name"].asCString();
            camera_item.type = cameraArray[k2]["type"].asInt();
            camera_item.state = cameraArray[k2]["state"].asInt();
            camera_item.url = cameraArray[k2]["url"].asCString();

            m_cameraList << camera_item;
        }
    }
    return true;
}

QList<DataSource::Location> JsonDataSource::getLocationList()
{
    return m_locationList;
}

QList<DataSource::Camera> JsonDataSource::getCameraList(const QString &location_obid)
{
    QList<DataSource::Camera> r;
    for(Camera c : m_cameraList) {
        if(c.location_obid == location_obid)
            r << c;
    }
    return r;
}

QList<DataSource::CameraState> JsonDataSource::getCameraStateList()
{
    return m_cameraStateList;
}

