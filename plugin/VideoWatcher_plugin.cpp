#include "VideoWatcher_plugin.h"
#include "pms/libPMS.h"
#include "pms/libPMS_Export.h"
#include <DBComponents.h>
#include <QtPlugin>
#include <QMessageBox>


void arg_message_handler(QtMsgType type, const char *msg)
{
//    QString txt;
//    switch (type) {
//    //调试信息提示
//    case QtDebugMsg:
//        txt = QString("Warning: %1").arg(msg);
//        break;
//        //一般的warning提示
//    case QtWarningMsg:
//        txt = QString("Warning: %1").arg(msg);
//        break;
//        //严重错误提示
//    case QtCriticalMsg:
//        txt = QString("Critical: %1").arg(msg);
//        break;
//        //致命错误提示
//    case QtFatalMsg:
//        txt = QString("Fatal: %1").arg(msg);
//        break;
//    }

    QString txt = msg;
    QString dt = "";//QTime::currentTime().toString("[mm:ss]");
    QString fi = "";//QString("[%1:$2]").arg(__FILE__).arg(__LINE__);
    QString tx = dt + fi + txt + "\n";

    QString filepath = "/home/cps/cz/log.txt";
    if(!QFile::exists(filepath)) {
        QFile file(filepath);
        file.open(QFile::WriteOnly);
        file.close();
    }
    //

    QFile file(filepath);
    file.open(QFile::Append);
    //file.write((QTime::currentTime().toString("[hh:mm:ss]") + txt + "\n").toLocal8Bit());
    file.write(tx.toLocal8Bit() + "\n");
    file.close();
}

class A {
public:
    A(){qInstallMsgHandler(arg_message_handler);}
};


controlButtonSettingDialog::controlButtonSettingDialog(VideoWatcherWidget* pWidget)
    : QDialog(pWidget){


    m_widget = pWidget;
    QGridLayout* g = new QGridLayout;
    QLabel* textLabel = new QLabel(QString::fromUtf8("按钮名称:"));
    QLabel* textsizeLabel = new QLabel(QString::fromUtf8("文字大小:"));
    text = new QLineEdit();
    textsize = new QLineEdit();
    g->addWidget(textLabel, 0, 0,1,1);
    g->addWidget(text, 0, 1,1,4);
    //g->addWidget(textsizeLabel, 0, 2,1,1);
    //g->addWidget(textsize, 0, 3,1,1);

    QLabel* objectLabel = new QLabel(QString::fromUtf8("目标对象:"));
    usedefault = new QCheckBox(QString::fromUtf8("是否使用动态关联对象"));
    g->addWidget(objectLabel, 1, 0,1,1);
    g->addWidget(usedefault, 1, 1,1,4);
    objectId = new QPushButton();
    g->addWidget(objectId, 2, 1,1,4);
    QLabel* attrLabel = new QLabel(QString::fromUtf8("操作属性:"));
    attribute = new QLineEdit();
    g->addWidget(attrLabel, 3, 0,1,1);
    g->addWidget(attribute, 3, 1,1,4);
    QLabel* valueLabel = new QLabel(QString::fromUtf8("属性值:"));
    attrvalue = new QLineEdit();
    g->addWidget(valueLabel, 4, 0,1,1);
    g->addWidget(attrvalue, 4, 1,1,4);

    QLabel* permitLabel = new QLabel(QString::fromUtf8("控制权限:"));
    permitId = new QPushButton();
    g->addWidget(permitLabel, 5, 0,1,1);
    g->addWidget(permitId, 5, 1,1,3);

    clearPermitBtn = new QPushButton(QString::fromUtf8("清除"));
    g->addWidget(clearPermitBtn, 5, 4,1,1);


    QLabel* logLabel = new QLabel(QString::fromUtf8("操作记录:"));
    logmsg = new QLineEdit();
    g->addWidget(logLabel, 6, 0,1,1);
    g->addWidget(logmsg, 6, 1,1,4);

    needconfirm = new QCheckBox(QString::fromUtf8("操作前是否需要确认？"));
    //g->addWidget(needconfirm, 7, 1,1,3);

    okButton = new QPushButton(QString::fromUtf8("确定"));
    g->addWidget(okButton, 7, 4,1,1);
    setLayout( g );
    setWindowTitle(QString::fromUtf8("操作按钮设置"));
    //setFixedSize();

    connect(usedefault,SIGNAL(stateChanged(int)),this,SLOT(onUserDefault(int)));
    connect(okButton,SIGNAL(clicked()),this,SLOT(doSave()));
    connect(objectId,SIGNAL(clicked()),this,SLOT(selectObject()));
    connect(permitId,SIGNAL(clicked()),this,SLOT(selectObject()));
    connect(clearPermitBtn,SIGNAL(clicked()),this,SLOT(selectObject()));
}

controlButtonSettingDialog::~controlButtonSettingDialog(){

}

void controlButtonSettingDialog::setData(QString m_data){
    try{
        QStringList fields = m_data.split("_");
        //qDebug()<<"controlButtonSettingDialog::setData"<<m_data;
        if (fields.count()==7){
            if(fields[0] == ""){
                text->setText("Button");
            }else{
                text->setText(fields[0]);
            }
            if(fields[2] == "defaultRuntime"){
                usedefault->setChecked(true);
            }else{
                usedefault->setChecked(false);
                objectId->setText(fields[2]);
            }
            attribute->setText(fields[3]);
            attrvalue->setText(fields[4]);
            needconfirm->setChecked(fields[5].toInt());
            logmsg->setText(fields[6]);
        }else if(fields.count()==8){
            if(fields[0] == ""){
                text->setText("Button");
            }else{
                text->setText(fields[0]);
            }
            if(fields[2] == "defaultRuntime"){
                usedefault->setChecked(true);
            }else{
                usedefault->setChecked(false);
                objectId->setText(fields[2]);
            }
            attribute->setText(fields[3]);
            attrvalue->setText(fields[4]);
            permitId->setText(fields[5]);
            needconfirm->setChecked(fields[6].toInt());
            logmsg->setText(fields[7]);
        }
    }catch(Exception& e){

    }
}

void controlButtonSettingDialog::onUserDefault(int state){
    if(state == 0){
        objectId->setEnabled(true);
    }else{
        objectId->setEnabled(false);
    }
}

void controlButtonSettingDialog::doSave(){
    try{
        Database* db = m_widget->getDatabase();
        QStringList temp;
        QString tempStr;
        tempStr = text->text().trimmed();
        if(tempStr != ""){
            temp.append(tempStr);
        }else{
            showError(text,QString::fromUtf8("请输入按钮名称！"));
            return;
        }
        temp.append("24");
        if(usedefault->isChecked()){
            temp.append("defaultRuntime");
        }else{
            if(objectId->text() == ""){
                showError(objectId,QString::fromUtf8("请选择操作的目标对象！"));
                return;
            }
            temp.append(objectId->text());
        }
        tempStr = attribute->text().trimmed();
        if(attrbuteCheck(db,tempStr)){
            temp.append(tempStr);
        }else{
            showError(attribute,QString::fromUtf8("请输入正确的属性名称！"));
            return;
        }
        tempStr = attrvalue->text().trimmed();
        if(tempStr != ""){
            temp.append(tempStr);
        }else{
            showError(attrvalue,QString::fromUtf8("请输入属性值！"));
            return;
        }
        tempStr = permitId->text();
        if(tempStr == "" || tempStr == "0"){
            temp.append("0");
        }else{
            temp.append(tempStr);
        }
        if(needconfirm->isChecked()){
            temp.append("1");
        }else{
            temp.append("0");
        }
        temp.append(logmsg->text());
        m_widget->setData(temp.join("_"));
        accept();
    }catch(Exception& e){

    }
}
bool controlButtonSettingDialog::attrbuteCheck(Database* db,QString attr){
    QString sep = attr.contains(QString(',')) ? ",":"->";
    QStringList list = attr.split(sep, QString::SkipEmptyParts);
    AType at;
    if(list.length() > 1){
        QStringListIterator javaStyleIterator(list);
        QString one;
        while (javaStyleIterator.hasNext()){
            one = javaStyleIterator.next();
            bool ret = false;
            int order = one.toInt(&ret,10);
            if(ret){
                if(order < 0) return false;
            }else{
                try{
                    at = db->matchAType(one.toUtf8().constData());
                }catch(Exception& e){
                    return false;
                }
            }
        }
    }else{
        try{
            at = db->matchAType(attr.toUtf8().constData());
        }catch(Exception& e){
            return false;
        }
    }
    return true;
};
void controlButtonSettingDialog::setLineEditeState(QLineEdit* le,bool state ){
    // 设置LineEdit变为红色
    if(state){
        le->setStyleSheet("QLineEdit{border:1px solid gray border-radius:1px}");
    }else{
        le->setStyleSheet("QLineEdit{border:1px solid red }");
    }
}
void controlButtonSettingDialog::showError(QWidget* le,QString msg ){
    // 错误提示
    QToolTip::showText(le->mapToGlobal(QPoint(100, 0)), msg);
}

void controlButtonSettingDialog::selectObject(){
    Database* pDatabase = m_widget->getDatabase();
    if (pDatabase==NULL)
        return;
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if(btn == clearPermitBtn){
        permitId->setText("");
        return;
    }
    ObId tempID;
    OType ot = 0;
    if(btn == objectId){
        tempID = objectId->text().toULongLong();
    }else{
        tempID = permitId->text().toULongLong();
        ot = pDatabase->matchOType("Permission");
    }
//    tempID = createNavigator(this,pDatabase, CONFIG_ENVIRONMENT, 0,ot, tempID);
//    if (tempID != 0) {
//        QString strOID = QString("%1").arg(tempID);
//        btn->setText(strOID);
//    }
}


////////////////////////////////////////////////////////////////////////////////
VideoWatcherPlugin::VideoWatcherPlugin(QObject *parent): QObject(parent)
{
    m_widget = NULL;
    m_dialog = NULL;

    QTimer::singleShot(1000, this, SLOT(startLog()));
}

VideoWatcherPlugin::~VideoWatcherPlugin()
{
    if(m_widget){
        delete m_widget;
        m_widget = NULL;
    }
    /*
    if(m_dialog){
        delete m_dialog;
        m_dialog = NULL;
    }*/
}

////////////////////////////////////////////////////////////////////////////////
QWidget *VideoWatcherPlugin::createWidget(QWidget *parent, Database *pDatabase, HistoricalClient *histClient, void *data, int environ)
{
    m_widget = new VideoWatcherWidget(parent, pDatabase, data, environ);
    return m_widget;
}

////////////////////////////////////////////////////////////////////////////////
bool VideoWatcherPlugin::popupWidget(QWidget *parent, void *data)
{
    if(m_widget == NULL){
        return false;
    }
    /*
    if(m_dialog == NULL){
        m_dialog = new controlButtonSettingDialog(m_widget);
    }*/
    this->m_widget = (VideoWatcherWidget*)parent;
    controlButtonSettingDialog* pDialog = new controlButtonSettingDialog(this->m_widget);
    pDialog->setData(m_widget->getData());
    if ( pDialog->exec() == QDialog::Accepted ){
        QString newData = this->m_widget->getData();
        memset(data,0,1000);
        strncpy((char*)data,newData.toUtf8().data(),newData.toUtf8().length());
        return 1 ;
    }
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
void VideoWatcherPlugin::setDataBase(Database *database)
{
    if (m_widget != NULL)
        m_widget->setDatabase(database);
}

//////////////////////////////////////////////////////////////////////////////
void VideoWatcherPlugin::setParent(QWidget *parent)
{
    if (m_widget != NULL)
        m_widget->setParent(parent);
}


void VideoWatcherPlugin::setInterfaces(UserMSInterface* userIface,OperationLog* olog){
    if (m_widget != NULL)
        m_widget->setInterfaces(userIface,olog);
}

////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherPlugin::name() const
{
    //必须和target的name相同
    return "videoWatcher_plugin";
}

////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherPlugin::group() const
{
    return QString::fromUtf8("视频监控");
}

////////////////////////////////////////////////////////////////////////////////
QIcon VideoWatcherPlugin::icon() const
{
    return QIcon(":/Resources/button.png");
}

////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherPlugin::toolTip() const
{
    return QString::fromUtf8("操作按钮");
}

////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherPlugin::whatsThis() const
{
    return QString::fromUtf8("操作按钮");
}

////////////////////////////////////////////////////////////////////////////////
bool VideoWatcherPlugin::isContainer() const
{
    return false;
}


////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherPlugin::domSvg() const
{
    return "output svg for test";
}

////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherPlugin::domG() const
{
    return "output g for test";
}

void VideoWatcherPlugin::startLog()
{
    static A static_aa;
}

////////////////////////////////////////////////////////////////////////////////
QString VideoWatcherPlugin::includeFile() const
{
    return "controlButton_plugin.h";
}

////////////////////////////////////////////////////////////////////////////////
void* VideoWatcherPlugin::getData()
{
    /*QMessageBox::information(NULL,"VideoWatcherPlugin::getData", "XXX");

    if (m_widget != NULL)
        return (void*)((VideoWatcherWidget*)m_widget)->getData().toUtf8().data();
    else*/
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void VideoWatcherPlugin::resize(int w, int h)
{
    if (m_widget != NULL)
        m_widget->resize(w, h);
}

////////////////////////////////////////////////////////////////////////////////



Q_EXPORT_PLUGIN2(VideoWatcherPlugin, VideoWatcherPlugin)

