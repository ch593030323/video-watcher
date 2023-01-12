#include "datasource.h"
#include "json/json.h"
#include "lds.h"

#include <QFile>

DataSource::DataSource(QObject *parent)
    : QObject(parent)
{

}

QByteArray DataSource::toJson()
{

    Json::Value settings;
    settings["default_column_count"] = lds::selectValue("select value from vw_settings where name = 'settings:%1' ", "default_column_count").toInt();
    settings["default_row_count"] = lds::selectValue("select value from vw_settings where name = 'settings:%1' ", "default_row_count").toInt();
    settings["default_video_url_list"] = lds::selectValue("select value from vw_settings where name = 'settings:%1' ", "default_video_url_list").toString().toStdString();

    Json::Value state_choice = Json::Value(Json::arrayValue);
    for(DataSource::CameraState state : this->getCameraStateList()) {
        Json::Value state_choiceValue;
        state_choiceValue["rank"] = state.rank;
        state_choiceValue["name"] = state.name.toStdString();
        state_choice.append(state_choiceValue);
    }

    Json::Value station = Json::Value(Json::arrayValue);
    for(DataSource::Location location : this->getLocationList()) {
        Json::Value stationValue;
        stationValue["obid"] = location.obid.toStdString();
        stationValue["name"] = location.name.toStdString();
        stationValue["camera"] = Json::Value(Json::arrayValue);

        for(DataSource::Camera camera : this->getCameraList(location.obid)) {
            Json::Value cameraValue;
            cameraValue["obid"] = camera.obid.toStdString();
            cameraValue["name"] = camera.name.toStdString();
            cameraValue["type"] = camera.type;
            cameraValue["state"] = camera.state;
            cameraValue["url"] = camera.url.toStdString();

            stationValue["camera"].append(cameraValue);
        }
        station.append(stationValue);
    }


    Json::Value root = Json::Value(Json::objectValue);
    Json::StyledWriter writer;

    Json::Value global = Json::Value(Json::objectValue);
    global["state_choice"] = state_choice;
    global["settings"] = settings;
    root["global"] = global;
    root["station"] = station;

    std::string json_file = writer.write(root);
    QString jsonString = QString::fromStdString(json_file).toLocal8Bit();

    return jsonString.toLocal8Bit();
}

void DataSource::fromJson(const QString &filepath)
{
    QSqlQuery query;
    QFile file(filepath);
    file.open(QFile::ReadOnly);
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

    Json::Value state_choice = value["global"]["state_choice"];
    for(int k = 0; k < state_choice.size(); k++) {
        query.exec(QString("replace into vw_settings values('state_choice:%1', '%2')")
                   .arg(state_choice[k]["rank"].asInt())
                   .arg(state_choice[k]["name"].asCString()));
    }

    Json::Value station = value["station"];
    for(int k1 = 0; k1 < station.size(); k1 ++) {
        Json::Value camera = station[k1]["camera"];
        for(int k2 = 0; k2 < camera.size(); k2 ++) {
            query.exec(QString("replace into vw_settings values('camera:%1:%2', '%3')")
                       .arg("state")
                       .arg(camera[k2]["obid"].asCString())
                    .arg(camera[k2]["state"].asInt()));

            query.exec(QString("replace into vw_settings values('camera:%1:%2', '%3')")
                       .arg("type")
                       .arg(camera[k2]["obid"].asCString())
                    .arg(camera[k2]["type"].asInt()));

            query.exec(QString("replace into vw_settings values('camera:%1:%2', '%3')")
                       .arg("url")
                       .arg(camera[k2]["obid"].asCString())
                    .arg(camera[k2]["url"].asCString()));
        }
    }
}

QString DataSource::getCameraStateName(int rank)
{
    for(DataSource::CameraState state : getCameraStateList()) {
        if(state.rank == rank) {
            return state.name;
        }
    }
    return "";
}

QString DataSource::getCameraTypeName(int rank)
{
    for(DataSource::CameraType state : getCameraTypeList()) {
        if(state.rank == rank) {
            return state.name;
        }
    }
    return "";
}

QList<DataSource::Location> DataSource::getLocationList()
{
    QList<DataSource::Location> r;
    r << DataSource::Location{"001", "station", 0, 0, 4};
    r << DataSource::Location{"002", "train",   1, 0, 4};

    return r;
}

QList<DataSource::Camera> DataSource::getCameraList(const QString &location_obid)
{
    QList<DataSource::Camera> r;
    if(location_obid == "001") {
        r << DataSource::Camera{"001000", "摄像头0", "001", 1, 1, "rtsp://admin:*Dt12a34b@192.7.3.159"};
        r << DataSource::Camera{"001001", "摄像头1", "001", 1, 1, "rtmp://10.137.32.250:1935/rtp/34020000001320000211_34020000001310000002"};
        r << DataSource::Camera{"001002", "摄像头2", "001", 1, 1, "rtsp://10.137.32.250:554/rtp/34020000001320000225_34020000001310000003"};
        r << DataSource::Camera{"001003", "摄像头3", "001", 1, 1, "rtsp://10.137.32.250:554/rtp/34020000001320000130_34020000001310000001"};
    }
    if(location_obid == "002") {
        r << DataSource::Camera{"002001", "摄像头4", "002", 1, 1, "http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4"};
        r << DataSource::Camera{"002002", "摄像头5", "002", 1, 1, "http://vfx.mtime.cn/Video/2021/01/07/mp4/210107172407759182_1080.mp4"};
        r << DataSource::Camera{"002003", "摄像头6", "002", 1, 1, "http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4"};
        r << DataSource::Camera{"002004", "摄像头7", "002", 1, 1, "http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp4"};
    }

    return r;
}

QList<DataSource::CameraState> DataSource::getCameraStateList()
{
    return QList<CameraState>()
            << CameraState{0, QString::fromUtf8("在线")}
            << CameraState{1, QString::fromUtf8("故障")}
            << CameraState{2, QString::fromUtf8("离线")};
}

QList<DataSource::CameraType> DataSource::getCameraTypeList()
{
    return QList<CameraType>()
            << CameraType{0, QString::fromUtf8("直机")}
            << CameraType{1, QString::fromUtf8("球机")};
}
