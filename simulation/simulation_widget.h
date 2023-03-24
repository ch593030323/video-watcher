#ifndef SIMULATION_WIDGET_H
#define SIMULATION_WIDGET_H

#include "src/mainwindow.h"
#include "src/lds.h"

#include <QtGui>
#include <oms/data_all.h>
#include <oms/qt_application.h>

#include <userms/UserMSInterface.h>
#include <operationlog/libOpLogInterface.h>


class GraphicComp;
class ScheDynamicManager;


class DataSourceSIM : public DataSource {
    Q_OBJECT
public:
    DataSourceSIM(OMSDatabase *database, const QString &PointAddress, const QString &Address, QObject *parent = 0);

    virtual QList<Location> getLocationList();
    virtual QList<Camera> getCameraList(const QString &location_obid);
    virtual QList<CameraState> getCameraStateList();
    virtual QList<CameraType> getCameraTypeList();

private:
    OMSDatabase *m_database;
    QString m_PointAddress;
    QString m_Address;
};

class SimulationWidget : public QWidget{
    Q_OBJECT

public:
    // ctor
    SimulationWidget(QWidget *parent, Database *database, void *data, int environ);

    // Databse handle in
    void setDatabase(Database *pDatabase);
    Database* getDatabase(void);

    // Parameter string retrieval
    void setData(QString data);
    QString getData(void);

    // Sizing & scaling
    virtual void  rescale(float scale)
    {
        QSize sz(scale*width(),scale*height());
        resize(sz);
    }
    void reSize(int w,int h){reSize(w,h);}
    // Chores for sche interactions
    virtual void exportImage(QString filename, int w, int h) const {}
    virtual void extractDynamicDefine(ScheDynamicManager *dm, GraphicComp *gc) const;

    bool visitChain(void);
    void setInterfaces(UserMSInterface*,OperationLog*);
    QString olbTextHandle();
    QString getAttributeValue(QString);
    bool checkPermitAndAor(QString&);
    bool getObIdByOrder(ObId parentId,AType atye,int order,ObId &tagetId);

private slots:
    void sendValueToOms(void);

private:
    Database    *m_pDatabase;
    OperationLog *m_pLog;
    UserMSInterface* m_pUserIface;
    QString     m_data;
    int         m_environ; // m_environ==1 in runtime

    /* Customized staff */
    QPushButton *m_button;
    ObId        m_obId;
    ObId        m_permitId;
    QString     m_permitName;//Permission
    ObId        m_writeId;
    AType       m_atypeId;
    QString     m_atypeText;
    DataType    m_attrType;
    QString     m_value;
    QString     m_confirm;
    QString     m_logMsg;
    bool        m_replaceNeed;

    QString CCTV = "10";
    QString STATION = "28";
};

#endif
