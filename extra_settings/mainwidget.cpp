#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "json/json.h"
#include "src/treevideowidget.h"
#include "src/treevideomodel.h"
#include "src/treevideoview.h"

#include <QFile>
#include <QSqlQuery>
#include <QtDebug>
#include <QTreeView>
#include <QItemSelectionModel>


mainWidget::mainWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    this->setObjectName("Window");

    m_datasource = new JsonDataSource("D:/Users/Dy/Documents/qt_project/build-VideoWatcher-Desktop_Qt_5_9_6_MinGW_32bit-Debug/video.json", this);
    ui->treeView->hideMenu();
    ui->treeView->setDataSource(m_datasource);

    m_tableModel = new QStandardItemModel(this);
    m_tableModel->setHorizontalHeaderLabels({"head"});
    ui->tableView->setModel(m_tableModel);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);



    connect(ui->treeView->m_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(slotUpdateInfo(QModelIndex, QModelIndex)));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_reload, SIGNAL(clicked()), this, SLOT(toreload()));
    connect(ui->pushButton_sub_defatult_video_url, SIGNAL(clicked()), this, SLOT(tosub_defatult_video_url()));
    connect(ui->pushButton_add_defatult_video_url, SIGNAL(clicked()), this, SLOT(toadd_defatult_video_url()));
    connect(ui->tableView->itemDelegate(), SIGNAL(closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint)),
            this, SLOT(toCheckTable(QWidget *, QAbstractItemDelegate::EndEditHint)));

    toreload();
}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::slotUpdateInfo(const QModelIndex &current, const QModelIndex &previous)
{
    ui->comboBox_state->setCurrentIndex(ui->comboBox_state->findData(current.data(VideoStateRole).toInt()));
    ui->comboBox_type->setCurrentIndex(ui->comboBox_type->findData(current.data(VideoTypeRole).toInt()));
    ui->plainTextEdit_url->setPlainText(current.data(VideoUrlRole).toString());
}

void mainWidget::toexit()
{
    this->close();
}

void mainWidget::toreload()
{
    m_datasource->update();
    ui->treeView->slotInitAll();

    ui->comboBox_state->clear();
    for(DataSource::CameraState state : m_datasource->getCameraStateList())
        ui->comboBox_state->addItem(state.name, state.rank);

    ui->comboBox_type->clear();
    for(DataSource::CameraType state : m_datasource->getCameraTypeList())
        ui->comboBox_type->addItem(state.name, state.rank);
}

void mainWidget::tosub_defatult_video_url()
{
    m_tableModel->removeRow(ui->tableView->currentIndex().row());
}

void mainWidget::toadd_defatult_video_url()
{
    int row = m_tableModel->rowCount() - 1;
    if(row >= 0 && m_tableModel->index(row, 0).data().toString().trimmed().isEmpty()) {
        m_tableModel->removeRow(row);
    }

    m_tableModel->insertRow(m_tableModel->rowCount());
    ui->tableView->selectRow(m_tableModel->rowCount() - 1);
}

void mainWidget::toCheckTable(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    if(editor && editor->property("text").toString().trimmed().isEmpty()) {
        m_tableModel->removeRow(m_tableModel->rowCount() - 1);
    }
}

JsonDataSource::JsonDataSource(const QString &jsonPath, QObject *parent)
    : DataSource(parent)
{
    m_jsonPath = jsonPath;
}

void JsonDataSource::update()
{
    QSqlQuery query;
    QFile file(m_jsonPath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
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
        QString location_obid = station[k1]["obid"].asCString();
        QString location_name = station[k1]["name"].asCString();
        Location location_item;
        location_item.obid = location_obid;
        location_item.name = location_name;

        m_locationList << location_item;

        Json::Value camera = station[k1]["camera"];
        for(int k2 = 0; k2 < camera.size(); k2 ++) {
            Camera camera_item;
            camera_item.location_obid = location_obid;
            camera_item.obid = camera[k2]["obid"].asCString();
            camera_item.name = camera[k2]["name"].asCString();
            camera_item.type = camera[k2]["type"].asInt();
            camera_item.state = camera[k2]["state"].asInt();
            camera_item.url = camera[k2]["url"].asCString();

            m_cameraList << camera_item;
        }
    }
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
