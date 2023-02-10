#include "lds.h"
#include "json/json.h"

#include <QFont>
#include <QApplication>
#include <QPainter>
#include <QFontDatabase>

//QRect lds::AppBoundingRect;
QString lds::IconFontFamily;

QColor lds::highlightColor;
QColor lds::textColor;
QColor lds::borderColor;
QColor lds::subToolColor;

QString lds::styleSheetString;

const int lds::margin = 10;
const int lds::marginX2 = 2 * lds::margin;
const int lds::border_width = 2;
const int lds::videoAreaRight = 280;
const int lds::videoAreaBottom = 50;

void lds::sethighlightColor(const QColor &color)
{
    highlightColor = color;
}

QColor lds::gethighlightColor() const
{
    return highlightColor;
}

void lds::settextColor(const QColor &color)
{
    textColor = color;
}

QColor lds::gettextColor() const
{
    return textColor;
}

void lds::setborderColor(const QColor &color)
{
    borderColor = color;
}

QColor lds::getborderColor() const
{
    return borderColor;
}

void lds::setsubToolColor(const QColor &color)
{
    subToolColor = color;
}

QColor lds::getsubToolColor() const
{
    return subToolColor;
}
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

QRect lds::MoveToCenter(QWidget *widget, QRect rect)
{
    int w = qMin(widget->width(), rect.width());
    int h = qMin(widget->height(), rect.height());
    return QRect(rect.center() - QPoint(w, h) / 2, QSize(w, h));
}

QPixmap lds::getFontPixmap(QChar ch, const QColor &color, const QSize &size)
{
    QFont font = qApp->font();
    font.setFamily(lds::IconFontFamily);
    font.setPixelSize(size.height());

    QPixmap pix(size);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setFont(font);
    painter.setPen(color);
    painter.drawText(pix.rect(), Qt::AlignCenter, ch);

    return pix;
}

QPixmap lds::getLayoutPixmap(int count)
{
    //默认绘制区域是size(100, 100)
    int c = count * 2 + 1;
    int w = 100 / c;
    QPainterPath path;
    for(int k = 0; k < count; k ++) {
        for(int k2 = 0; k2 < count; k2 ++) {
            path.addEllipse((2 * k + 1) * w, (2 * k2 + 1) *w, w, w);
        }
    }
    QPixmap pixmap(100, 100);
    pixmap.fill(Qt::transparent);
    QPainter paint(&pixmap);
    paint.fillPath(path, lds::textColor);

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
    IconFontFamily = QFontDatabase::applicationFontFamilies(fontId).value(0);

    qDebug() << "IconFontFamily:" << IconFontFamily << QFontDatabase::applicationFontFamilies(fontId) << fontId;

    //skin
    QFile file(":/skin_light.qss");
//    QFile file("skin.qss");
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << "file open error:" << file.errorString();
    }

    styleSheetString = file.readAll();
    //    qApp->setStyleSheet(styleSheetString);

    lds qsswidget;
    qsswidget.setStyleSheet(styleSheetString);
    qsswidget.style()->polish(&qsswidget);

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

    qDebug() << __LINE__ << query.exec("create table vw_device ("
                                       "obid varchar(20) primary key, "
                                       "name varchar(20), "
                                       "location_obid varchar(20), "
                                       "type int,"
                                       "state int,"
                                       "url varchar(20)"
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
