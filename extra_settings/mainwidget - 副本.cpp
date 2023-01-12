#include "mainwidget.h"
#include "json/json.h"
#include "src/treevideowidget.h"

#include <QFile>
#include <QSqlQuery>
#include <QtDebug>

mainWidget::mainWidget(QWidget *parent)
    : QMainWindow(parent)
{
    JsonDataSource *datasource = new JsonDataSource("D:/Users/Dy/Documents/qt_project/build-VideoWatcher-Desktop_Qt_5_9_6_MinGW_32bit-Debug/video.json", this);
    TreeVideoWidget *view =  new TreeVideoWidget(this);
    view->setDataSource(datasource);
    view->setGeometry(0, 0, 400, 800);

    datasource->update();
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
