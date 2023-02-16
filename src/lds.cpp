#include "lds.h"
#include "json/json.h"
#include "propertycolor.h"

#include <QFont>
#include <QApplication>
#include <QPainter>
#include <QFontDatabase>

//QRect lds::AppBoundingRect;
QString lds::iconFontFamily;
QString lds::styleSheetString;

const int lds::margin = 10;
const int lds::marginX2 = 2 * lds::margin;
const int lds::border_width = 2;
const int lds::videoAreaRight = 280;
const int lds::videoAreaBottom = 50;

QVariant lds::selectValue(const QString &sql, const QString &arg0, const QVariant &def)
{
    QSqlQuery query;
    query.exec(sql.arg(arg0));
    if(query.next())
        return query.record().value(0);
    return def;
}

QVariant lds::selectValue(const QString &sql, int arg0, const QVariant &def)
{
    QSqlQuery query;
    query.exec(sql.arg(arg0));
    if(query.next())
        return query.record().value(0);
    return def;
}

lds::lds(QWidget *parent) : QWidget(parent)
{
    setObjectName("lds");
}

QRect lds::moveToCenter(QWidget *widget, QRect rect)
{
    int w = qMin(widget->width(), rect.width());
    int h = qMin(widget->height(), rect.height());
    return QRect(rect.center() - QPoint(w, h) / 2, QSize(w, h));
}

QPixmap lds::getFontPixmap(QChar ch, const QColor &color, const QSize &size)
{
    return PropertyColor::getFontPixmap(ch, color, size);
}

QPixmap lds::getLayoutPixmap(int count)
{
    //默认绘制区域是size(100, 100)
    int size = 100;
    int c = count * 2 + 1;
    int w = size / c;
    QPainterPath path;
    for(int k = 0; k < count; k ++) {
        for(int k2 = 0; k2 < count; k2 ++) {
            path.addEllipse((2 * k + 1) * w, (2 * k2 + 1) *w, w, w);
        }
    }
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QPainter paint(&pixmap);
    paint.fillPath(path, PropertyColor::buttonTextColor);

    return pixmap;
}

void lds::init()
{
    //text code
    //    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    //    QTextCodec::setCodecForLocale(codec);

    //#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    //#else
    //    QTextCodec::setCodecForCStrings(codec);
    //    QTextCodec::setCodecForTr(codec);
    //#endif

    //bounding rect
    //    lds::AppBoundingRect = qApp->desktop()->availableGeometry()
    //            .adjusted(0, 30, 0, 0) //substract title height
    //            .adjusted(30, 30, -30, -30);//substract margin 10 px

    //icon font
    int fontId = QFontDatabase::addApplicationFont(":/Awesome.otf");
    iconFontFamily = QFontDatabase::applicationFontFamilies(fontId).value(0);

    qDebug() << "iconFontFamily:" << iconFontFamily << QFontDatabase::applicationFontFamilies(fontId) << fontId;

    //skin
//    QFile file(":/skin.qss");
    QFile file(":/skin_light.qss");
//    QFile file("skin.qss");
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << "file open error:" << file.errorString();
    }

    styleSheetString = file.readAll();
    //    qApp->setStyleSheet(styleSheetString);

    PropertyColor::init(iconFontFamily, styleSheetString);

    //font
    //    QFont font = qApp->font();
    //    font.setPixelSize(25);
    //    qApp->setFont(font);

    /*
表：vw_location
[obid:001---key]	[name:A1]	[type:0：station、1：train] [state 0:no device 1 has device]

表：vw_device
[obid:001001---key	name:"摄像头1"]	[location_obid:001]	[type:1：直机、2：球机]	[state:1：在线、2：离线、3：故障]	[url:视频流地址]
*/
    //database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if(!db.open()) {
        qDebug() << "db open error:" << db.lastError().text();
    }
    QSqlQuery query;
    qDebug() << __LINE__ << query.exec("create table vw_location ("
                                       "obid varchar(20) primary key, "
                                       "name varchar(20), "
                                       "type int,"
                                       "state int)"
                                       );

    /*
     * 软件treeviwe的节点展开采用动态记载的方式
     * 因为treeview当没有child时，是无法显示节点图标的，这时要创建个【device_id为空的child作为占位用】（显示location的节点）
     * 采用复合组件的方式PRIMARY KEY(obid, location_obid), obid+location_obid不重复即可，来实现【device_id为空的child作为占位用】
     */
    qDebug() << __LINE__ << query.exec("create table vw_device ("
                                       "obid varchar(20) , "
                                       "name varchar(20), "
                                       "location_obid varchar(20) , "
                                       "type int,"
                                       "state int,"
                                       "url varchar(20),"
                                       "PRIMARY KEY(obid, location_obid)"
                                       ")"
                                       );
    qDebug() << __LINE__ << query.exec("create table vw_device_state ("
                                       "rank varchar(20) primary key, "
                                       "name varchar(20)"
                                       ")"
                                       );
    qDebug() << __LINE__ << query.exec("create table vw_settings ("
                                       "name varchar(20) primary key, "
                                       "value varchar(20)"
                                       ")"
                                       );
}

QList<AlterPlayFrame> AlterPlayFrame::readFrom(const QString &filepath)
{
    QList<AlterPlayFrame> playlist;
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
    }
    while(!file.atEnd()) {
        QString line = file.readLine();
        QString line2 = file.readLine();

        AlterPlayFrame frame;
        frame.device_obid = line.mid(2).trimmed();
        frame.timeout = line2.mid(2).trimmed().toInt();

        playlist << frame;
    }
    return playlist;
}
