#include "datasource.h"
#include "json/json.h"
#include "lds.h"

#include <QFile>

DataSource::DataSource(QObject *parent)
    : QObject(parent)
{

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
