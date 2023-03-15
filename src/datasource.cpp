#include "datasource.h"
#include "json/json.h"
#include "lds.h"
#include "jsoncpppath.h"

#include <QFile>
#include <QVariant>
#include <QtDebug>

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

QString DataSource::getCameraUrl(const QString &obid)
{
    if(m_urlCache.isEmpty()) {
        JsonCppSettings p(lds::configDirectory + "/setting.json");
        m_urlCache = p.value("/device_list").toMap();
    }
    if(!m_urlDirty.isEmpty()) {
        cacheInsert(m_urlDirty);
        dirtyClear();
    }
    QString url = m_urlCache.value(obid).toString();

    return url;
}

QList<DataSource::Location> DataSource::getLocationList()
{
    QList<DataSource::Location> r;
    r << DataSource::Location{"001", "station", 0, 0, 4};
    r << DataSource::Location{"002", "train",   1, 0, 4};

    return r;
}

/*
        "001000": "rtsp://admin:*Dt12a34b@192.7.3.159",
        "001001": "rtmp://10.137.32.250:1935/rtp/34020000001320000211_34020000001310000002",
        "001002": "rtsp://10.137.32.250:554/rtp/34020000001320000225_34020000001310000003",
        "001003": "rtsp://10.137.32.250:554/rtp/34020000001320000130_34020000001310000001",
        "002001": "http://vfx.mtime.cn/Video/2019/03/14/mp4/190314223540373995.mp4",
        "002002": "http://vfx.mtime.cn/Video/2021/01/07/mp4/210107172407759182_1080.mp4",
        "002003": "http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4",
        "002004": "http://vfx.mtime.cn/Video/2019/03/17/mp4/190317150237409904.mp4"
*/
QList<DataSource::Camera> DataSource::getCameraList(const QString &location_obid)
{
    QList<DataSource::Camera> r;
    if(location_obid == "001") {
        r << DataSource::Camera{"001000", "摄像头0", "001", 1, 1};
        r << DataSource::Camera{"001001", "摄像头1", "001", 1, 1};
        r << DataSource::Camera{"001002", "摄像头2", "001", 1, 1};
        r << DataSource::Camera{"001003", "摄像头3", "001", 1, 1};
    }
    if(location_obid == "002") {
        r << DataSource::Camera{"002001", "摄像头4", "002", 1, 1};
        r << DataSource::Camera{"002002", "摄像头5", "002", 1, 1};
        r << DataSource::Camera{"002003", "摄像头6", "002", 1, 1};
        r << DataSource::Camera{"002004", "摄像头7", "002", 1, 1};
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

void DataSource::cacheSave()
{
    JsonCppSettings p(lds::configDirectory + "/setting.json");
    p.setValue("/device_list", m_urlCache);
    if(!p.saveToFile()) {
        qDebug() << p.errorString();
    }
}

void DataSource::cacheInsert(QMap<QString, QVariant> map)
{
    for(QMap<QString, QVariant>::const_iterator k = map.constBegin(); k != map.constEnd(); k ++) {
        m_urlCache.insert(k.key(), k.value());
    }
}

QMap<QString, QVariant> DataSource::urlCache()
{
    return m_urlCache;
}

void DataSource::dirtyInsert(QString key, QVariant value)
{
    m_urlDirty.insert(key, value);
}

void DataSource::dirtyClear()
{
    m_urlDirty.clear();
}

QMap<QString, QVariant> DataSource::dirtyUrl()
{
    return m_urlDirty;
}
