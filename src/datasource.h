#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>

class DataSource : public QObject{
    Q_OBJECT
public:
    struct Location {
        //for sql
        QString obid;
        QString name;
        int type;//车站的类型，0：车站、1：列车 【暂作废】

        //for var
        int state;//state 0:no device 1 has device 默认是0，加载一个节点后改为1，刷新时需要改成0才能刷新
        int camera_count;//从oms加载节点时，会读取站下面有多个设备，不保存到sql里
    };

    struct Camera {
        //for sql
        QString obid;
        QString name;
        QString location_obid;
        int type;//摄像头类型:1：直机、2：球机
        int state;//摄像头状态:1：在线、2：离线、3：故障
        QString url;//视频流地址

    };
    struct CameraState {
        int rank;
        QString name;
    };

    struct CameraType {
        int rank;
        QString name;
    };
    DataSource(QObject *parent = 0);

    QString getCameraStateName(int rank);
    QString getCameraTypeName(int rank);
    //
    virtual QList<Location> getLocationList();
    virtual QList<Camera> getCameraList(const QString &location_obid);
    virtual QList<CameraState> getCameraStateList();
    virtual QList<CameraType> getCameraTypeList();
};


#endif // DATASOURCE_H
