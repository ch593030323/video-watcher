#include "VideoWatcher_widget.h"
#include <oms/request.h>
#include <oms/database.h>
//#include <Helper/Attribute/dynamicManager.h>
#include "omsquery.h"
#include "ui_mainwindow.h"
#include "src/treevideomodel.h"

#include <QSqlError>

////////////////////////////////////////////////////////////////////////////////

VideoWatcherWidget::VideoWatcherWidget(QWidget *parent, Database *database, void *data, int environ)
    : QWidget(parent)
    , m_environ(environ)
    , m_pDatabase(database)
{
    // UI chores
    setDatabase(database);
    char* para = (char* )data;
    setData(QString::fromUtf8(para));


    //    QVBoxLayout* layout = new QVBoxLayout(this);
    m_button = new QPushButton("Button2",this);
    m_button->hide();
    //    m_button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    //    m_button->hide();

    //    layout->addWidget(new MainFrame, 1);
    //    setLayout(layout);

    m_replaceNeed = false;
    m_obId = 0;
    m_permitId = 0;
    m_atypeId = 0;
    //    connect(m_button, SIGNAL(clicked (bool)), this, SLOT(sendValueToOms()));
    QVBoxLayout *vlayout = new QVBoxLayout;
    MainFrame *window = new MainFrame;
    vlayout->addWidget(window);
    vlayout->setMargin(0);
    this->setLayout(vlayout);
    window->setDataSource(new DataSourceOMS(static_cast<OMSDatabase *>(database), CCTV, STATION, this));
};


////////////////////////////////////////////////////////////////////////////////
void VideoWatcherWidget::setDatabase(Database *database)
{
    m_pDatabase = database;
    OMSDatabase *d = static_cast<OMSDatabase *>(m_pDatabase);

    OMSQuery query(d);
    query.test();
    //    ChoiceData data;
    //    StringData stringData;
    //    AType att_atype = d->matchAType("Name");
    //    ObId id = 12884915094;
    //    std::cout << __LINE__ << att_atype << std::endl;
    //    Request req(id, att_atype, static_cast<Data*>(&stringData));
    //    d->read(&req, 1);

    //    std::cout << __LINE__ << " : " << QString::fromStdString(stringData).toStdString() << std::endl;
    ////    std::cout << __LINE__ << std::endl;
    ////    for(int k = 0 ; k < data.getNumberOfChoices(); k++) {
    ////        std::cout << data.getObIds()[k] << data.getStrings()[k] << std::endl;
    ////    }
}

////////////////////////////////////////////////////////////////////////////////
Database* VideoWatcherWidget::getDatabase(void)
{
    return m_pDatabase;
}



////////////////////////////////////////////////////////////////////////////////
void VideoWatcherWidget::setData(QString data)
{
    try{
        //qDebug()<<"VideoWatcherWidget::setData:"<<data;//wzx
        m_data = data;
        QStringList fields = m_data.split("_");
        if (fields.count()==7){
            m_button->setText(fields[0]);
            if(m_environ == 1){
                if(fields[2] == "defaultRuntime"){
                    m_replaceNeed = true;
                }else{
                    m_obId = fields[2].toLong();
                }
                m_atypeText = fields[3];
                if(m_obId !=0){
                    visitChain();
                    m_button->setEnabled(true);
                    //qDebug()<<"m_writeId:"<<m_writeId<<"m_atypeId:"<<m_atypeId;
                }else{
                    m_button->setEnabled(false);
                }
                m_value = fields[4];
                m_confirm = fields[5];
                m_logMsg = fields[6];
            }
        }else if(fields.count()==8){
            m_button->setText(fields[0]);
            if(m_environ == 1){
                if(fields[2] == "defaultRuntime"){
                    m_replaceNeed = true;
                }else{
                    m_obId = fields[2].toLong();
                }
                m_atypeText = fields[3];
                if(m_obId !=0){
                    visitChain();
                    //qDebug()<<"m_writeId:"<<m_writeId<<"m_atypeId:"<<m_atypeId;
                }
                m_value = fields[4];
                m_permitId =  fields[5].toLong();
                if(m_permitId>0){
                    AType at_name = m_pDatabase->matchAType("Name");
                    StringData strdata;
                    try{
                        m_pDatabase->read(m_permitId, at_name, &strdata);
                    }catch (Exception e){
                    }
                    std::string temp = (std::string)strdata;
                    m_permitName = QString::fromUtf8(temp.c_str());
                }
                m_confirm = fields[6];
                m_logMsg = fields[7];
            }
        }

    }catch(Exception& e){

    }
}

////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherWidget::getData(void)
{
    //qDebug()<<"VideoWatcherWidget::getData:"<<m_data;
    return m_data;
}

void VideoWatcherWidget::setInterfaces(UserMSInterface* userIface,OperationLog* olog){
    //qDebug()<<"VideoWatcherWidget::setInterfaces";//wzx
    m_pUserIface = userIface;
    m_pLog = olog;
}

bool VideoWatcherWidget::getObIdByOrder(ObId parentId,AType atye,int order,ObId &tagetId){
    if(order < 0){
        return false;
    }
    ContainerData containerData;
    AType atObj;
    try{
        atObj = m_pDatabase->matchAType("ObjOrder");
        Request req( parentId, atye,  (Data*)&containerData);
        m_pDatabase->read(&req,1);
    }
    catch(Exception e){
        return false;
    }
    int num = containerData.getNumberOfElements();
    if(num == 0){
        return false;
    }
    const ObId* obIds = containerData.getObIds();
    if(num ==1 && order == 0){
        tagetId = *obIds;
        return true;
    }
    IntegerData orderData = -1;
    for(int j = 0 ; j < num ; j++)
    {
        ObId id_Child = *(obIds+j);
        try{
            m_pDatabase->read(id_Child,atObj,&orderData);
        }
        catch(Exception e){
            return false;
        }
        if((int)orderData == order){
            tagetId = id_Child;
            return true;
        }
    }
    return false;
}


bool VideoWatcherWidget::visitChain(){
    QString sep = m_atypeText.contains(QString(',')) ? ",":"->";
    QStringList list = m_atypeText.split(sep, QString::SkipEmptyParts);
    if(list.length() > 1){
        m_writeId = m_obId;
        AType at;
        QStringListIterator javaStyleIterator(list);
        QString one;
        /*char chtext[] = { 0xE6,0x9C,0xAA,0xE4,0xBD,0xBF,0xE7,0x94,0xA8,'\0'};
        QString notusedstr = QString::fromUtf8((char*)chtext);
        */

        while (javaStyleIterator.hasNext()){
            one = javaStyleIterator.next();
            try{
                at = m_pDatabase->matchAType(one.toUtf8().constData());
            }catch(Exception& e){
                OMString str = e.getDescription();
                QMessageBox::warning(0, QObject::tr("database error"), QString("parseAtypeName %1 error, ").arg(m_atypeText) + QString::fromUtf8(str.c_str()));
                m_atypeId = 0;
                return false;
            }
            m_atypeId = at;
            m_attrType = m_pDatabase->matchDataType(m_atypeId);
            if(m_attrType == LINK_DATA){
                try{
                    LinkData tempData;
                    Request req( m_writeId ,m_atypeId ,  (Data*)&tempData);
                    m_pDatabase->read(&req,1);
                    m_writeId = (ObId)tempData;
                }catch (Exception e){
                    m_atypeId = 0;
                    return false;
                }
            }else if(m_attrType == CONTAINER_DATA){
                if(!javaStyleIterator.hasNext())
                    return false;
                one = javaStyleIterator.next();
                bool ret = false;
                int order = one.toInt(&ret,10);
                if(!ret) return false;
                if(!getObIdByOrder(m_writeId,m_atypeId,order,m_writeId))
                    return false;
            }
        }
    }else{
        try{
            m_writeId = m_obId;
            m_atypeId = m_pDatabase->matchAType(m_atypeText.toUtf8().constData());
            m_attrType = m_pDatabase->matchDataType(m_atypeId);
        }catch(Exception& e){
            OMString str = e.getDescription();
            QMessageBox::warning(0, QObject::tr("database error"), QString("parseAtypeName %1 error, ").arg(m_atypeText) + QString::fromUtf8(str.c_str()));
            m_atypeId = 0;
            return false;
        }

    }
    return true;
}




////////////////////////////////////////////////////////////////////////////////
void VideoWatcherWidget::extractDynamicDefine(ScheDynamicManager *dm, GraphicComp *gc) const
{
    return;
    /*
    if ((dm == NULL) || (gc == NULL))
        return;

    if (m_environ == RUNTIME_ENVIRONMENT)
    {
        AType atCPUSystem = m_pDatabase->matchAType("CPUSystem");
        //QMessageBox::information(NULL, QString("before"), QString("%1.%2").arg(m_obId).arg(atCPUSystem));
        dm->addDynamicItem(m_obId, atCPUSystem, gc);
        QMessageBox::information(NULL, QString("after").arg(m_obId), QString("%1").arg(atCPUSystem));
    }*/
}

QString VideoWatcherWidget::olbTextHandle(){
    try{
        QString str = m_logMsg.trimmed();
        //qDebug()<<"olbTextHandle"<<str;//wzx
        QStringList tempList;
        int pos = str.indexOf("%A[");
        while(pos != -1){
            int pos2 = str.indexOf("]",pos+3);
            if(pos2 ==-1) break;
            tempList.append(str.mid(pos+3,pos2-pos-3));
            pos = str.indexOf("%A[",pos2+1);
        }
        foreach(QString temp,tempList){
            str.replace(QString("%A[%1]").arg(temp),getAttributeValue(temp));
        }
        tempList.clear();
        return str;
    }catch(Exception& e){
        return "";
    }
}

QString VideoWatcherWidget::getAttributeValue(QString attrStr){
    ObId tempId;
    DataType dt;
    AType at;
    tempId = m_obId;
    QString tempStr;
    QString sep = attrStr.contains(QString(',')) ? ",":"->";
    QStringList list = attrStr.split(sep, QString::SkipEmptyParts);
    if(list.length() > 1){
        QStringListIterator javaStyleIterator(list);
        QString one;
        /*char chtext[] = { 0xE6,0x9C,0xAA,0xE4,0xBD,0xBF,0xE7,0x94,0xA8,'\0'};
        QString notusedstr = QString::fromUtf8((char*)chtext);
        */

        while (javaStyleIterator.hasNext()){
            one = javaStyleIterator.next();
            try{
                at = m_pDatabase->matchAType(one.toUtf8().constData());
            }catch(Exception& e){
                OMString str = e.getDescription();
                QMessageBox::warning(0, QObject::tr("database error"), QString("parseAtypeName %1 error, ").arg(m_atypeText) + QString::fromUtf8(str.c_str()));
                return attrStr;
            }
            dt = m_pDatabase->matchDataType(at);
            if(dt == LINK_DATA){
                try{
                    LinkData tempData;
                    m_pDatabase->read(tempId, at, &tempData);
                    tempId = (ObId)tempData;
                }catch (Exception e){
                    return attrStr;
                }
            }else if(dt == CONTAINER_DATA){
                if(!javaStyleIterator.hasNext())
                    return attrStr;
                one = javaStyleIterator.next();
                bool ret = false;
                int order = one.toInt(&ret,10);
                if(!ret) return attrStr;
                if(!getObIdByOrder(tempId,at,order,tempId)) return attrStr;
            }
        }
    }else{
        try{
            at = m_pDatabase->matchAType(attrStr.toUtf8().constData());
            dt = m_pDatabase->matchDataType(at);
        }catch(Exception& e){
            OMString str = e.getDescription();
            QMessageBox::warning(0, QObject::tr("database error"), QString("parseAtypeName %1 error, ").arg(m_atypeText) + QString::fromUtf8(str.c_str()));
            return attrStr;
        }

    }
    if(at !=0){
        try{
            if(dt == BOOLEAN_DATA || dt == SHORT_DATA || dt == MULTIPLE_CHOICE_DATA){
                IntegerData v;
                m_pDatabase->read(tempId, at, &v);
                tempStr = QString::number(v);
            }else if(dt == FLOAT_DATA){
                FloatData v;
                m_pDatabase->read(tempId, at, &v);
                tempStr = QString::number(v);
            }else{
                StringData v;
                m_pDatabase->read(tempId, at, &v);
                std::string s=  (std::string)v;
                tempStr = QString::fromUtf8(s.c_str());
            }
        }catch(Exception& e){
            return attrStr;
        }
        return tempStr;
    }else{
        return attrStr;
    }
}


void VideoWatcherWidget::sendValueToOms(void)
{
    //qDebug()<<"VideoWatcherWidget::sendValueToOms,m_environ:"<<m_environ;//wzx
    //qDebug()<<"m_writeId:"<<m_writeId<<";m_atypeId:"<<m_atypeId<<"value:"<<m_value;//wzx
    //qDebug()<<"m_confirm:"<<m_confirm;//wzx
    //qDebug()<<"m_attrType:"<<m_attrType;//wzx
    if(m_environ != 1){
        return;
    }
    QString msg = "",oblText = "";
    bool flag = checkPermitAndAor(msg);
    ObId userId = DEFAULT_USER_OBJECT_ID;
    //qDebug()<<"checkPermitAndAor finished===============";//wzx
    if(flag){
        flag = false;
        if(m_writeId == 0 || m_atypeId == 0){
            msg = QString::fromUtf8("无效的数据对象！");
        }else{
            oblText = olbTextHandle();
            if(m_pUserIface && m_pUserIface->isInitialized()){
                userId = m_pUserIface->getSystemUser();
                //qDebug()<<"userId=========="<<userId;//wzx
            }
            try{
                if(m_attrType == BOOLEAN_DATA || m_attrType == SHORT_DATA || m_attrType == MULTIPLE_CHOICE_DATA){
                    IntegerData v = m_value.toInt();
                    m_pDatabase->write(m_writeId, m_atypeId, &v, NORMAL_WRITE,NULL,NULL,userId);
                    flag = true;
                }else if(m_attrType == FLOAT_DATA){
                    FloatData v = m_value.toFloat();
                    m_pDatabase->write(m_writeId, m_atypeId, &v, NORMAL_WRITE,NULL,NULL,userId);
                    flag = true;
                }else if(m_attrType == STRING_DATA){
                    StringData v(m_value.toUtf8().data());
                    m_pDatabase->write(m_writeId, m_atypeId, &v, NORMAL_WRITE,NULL,NULL,userId);
                }
                else{
                    msg = QString::fromUtf8("无效的数据类型！");
                }
            }catch(Exception& e){
                msg = QString::fromUtf8("操作失败！");
            }
        }
    }
    if(flag){
        //qDebug()<<"writeData==============finish";//wzx
        //qDebug()<<"oblText=============="<<oblText;
        //qDebug()<<"m_pLog->isInitialized()"<<m_pLog<<m_pLog->isInitialized();
        if(oblText == ""){
            oblText = "write to oms"+QString::number(m_writeId)+","+QString::number(m_atypeId)+","+m_value;
        }
        try{
            //            if(!m_pLog->writeLog(oblText,SYS_LOG_INFO,MMI_CTRL,m_obId)){
            //                ACE_DEBUG ((LM_INFO,  ACE_TEXT ("(%P) %D %n ControlButton_Plugin, write obl failed! device:%Q,userId:%Q,obl:%s \n"),m_obId,userId,oblText.toUtf8().data()));
            //            }
        }catch(Exception& e){
            qDebug()<<"writeLog==============failed!";
        }
        //qDebug()<<"writeLog==============finish!";
        //QToolTip::showText(m_button->mapToGlobal(QPoint(5, 0)), msg);
    }else{
        QToolTip::showText(m_button->mapToGlobal(QPoint(5, 0)), msg);
    }

    //qDebug()<<"VideoWatcherWidget::sendValueToOms==============end";//wzx
}

bool VideoWatcherWidget::checkPermitAndAor(QString& msg){
    bool flag = true;
    if(flag && !m_pUserIface->ds6k_isObjectInUserAORSet(m_obId)){
        //qDebug()<<"check aor ...";//wzx
        msg = QString::fromUtf8("当前用户不具有被控对象的责任区!");
        flag = false;
    }
    if(flag && m_permitId>0 && !m_pUserIface->ds6k_isPermitted(m_permitId)){
        //qDebug()<<"check permit ...";//wzx
        msg = QString::fromUtf8("当前用户不具有[%1]控制权限!").arg(m_permitName);
        flag = false;
    }
    return flag;
}

DataSourceOMS::DataSourceOMS(OMSDatabase *database, const QString &PointAddress, const QString &Address, QObject *parent)
    : DataSource(parent)
{
    m_database = database;
    m_PointAddress = PointAddress;
    m_Address = Address;
}

QList<DataSource::Location> DataSourceOMS::getLocationList()
{
    QList<DataSource::Location> r;
    OMSQuery query_camera(m_database);
    OMSQuery query_cctv(m_database);
    query_cctv.exec(QString("select obid, Name from CCTVController where PointAddress = %1  ")
                    .arg(m_PointAddress));
    while(query_cctv.next()) {
        QString obid = query_cctv.value("obid").toString();
        QString name = QString::fromUtf8(query_cctv.value("Name").toByteArray().data());
        query_camera.exec(QString("select count from Camera where ParentLink = %1")
                          .arg(obid));
        query_camera.next();
        int count = query_camera.value(0).toInt();

        DataSource::Location d;
        d.obid = obid;
        d.name = name;
        d.type = 0;
        d.state = 0;
        d.camera_count = count;

        r << d;
    }

    return r;
}

QList<DataSource::Camera> DataSourceOMS::getCameraList(const QString &location_obid)
{
    QList<DataSource::Camera> r;
    OMSQuery query(m_database);
    query.exec(QString("select obid, Name, FaultState from Camera where ParentLink = %1")
                      .arg(location_obid));
    while(query.next()) {
        DataSource::Camera d;
        d.obid = query.value("obid").toString();
        d.name = QString::fromUtf8(query.value("Name").toByteArray());
        d.location_obid = location_obid;
        d.type = 1;
        d.state = query.value("FaultState").toInt();
        //d.url = "rtsp://10.137.32.250:554/rtp/34020000001320000067_34020000001310000001";

        r << d;
    }

    return r;
}

QList<DataSource::CameraState> DataSourceOMS::getCameraStateList()
{
    QList<DataSource::CameraState> r;
    OMSQuery query(m_database);
    query.serialPrepare();
    query.serialExec("select obid, Name from ObjectType where Name = Camera");
    query.serialExec("select ObjectSpecifier,Name from ObjectAttribute "
                     "where Name = FaultState and ParentLink=?");
    query.serialExec("select obid, Name, Rank from Choice where ParentLink=?");
    while(query.next()) {
        DataSource::CameraState d;
        d.rank = query.value("Rank").toInt();
        d.name = query.value("Name").toString();
        r << d;
    }
    return r;
}

QList<DataSource::CameraType> DataSourceOMS::getCameraTypeList()
{
    QList<DataSource::CameraType> r;
    OMSQuery query(m_database);
    query.serialPrepare();
    query.serialExec("select obid, Name from ObjectType where Name = Camera");
    query.serialExec("select ObjectSpecifier,Name from ObjectAttribute "
                     "where Name = Type and ParentLink=?");
    query.serialExec("select obid, Name, Rank from Choice where ParentLink=?");
    while(query.next()) {
        DataSource::CameraType d;
        d.rank = query.value("Rank").toInt();
        d.name = query.value("Name").toString();
        r << d;
    }
    return r;
}
