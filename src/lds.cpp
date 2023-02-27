#include "lds.h"
#include "json/json.h"
#include "propertycolor.h"

#include <QFont>
#include <QApplication>
#include <QPainter>
#include <QFontDatabase>
#include <QtDebug>
#include <QFile>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

//QRect lds::AppBoundingRect;
QString lds::iconFontFamily;
QString lds::styleSheetString;

const int lds::margin = 10;
const int lds::marginX2 = 2 * lds::margin;
const int lds::border_width = 2;
const int lds::videoAreaRight = 280;
const int lds::videoAreaBottom = 50;

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

QString lds::getUniqueFileNamehByDateTime(const QString &dir)
{
    //自动填充新增的文件名
    int maxIndex = -1;
    QString prefix = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm");
    for(const QFileInfo &info : QDir(dir).entryInfoList()) {
        if(info.isFile() && info.baseName().startsWith(prefix)) {
            int begin = info.baseName().lastIndexOf("(");
            int end = info.baseName().lastIndexOf(")");
            maxIndex = qMax(maxIndex, info.baseName().mid(begin + 1, end - begin - 1).toInt());
        }
    }
    maxIndex++;

    return (maxIndex == 0 ? prefix : (prefix + "(" + QString::number(maxIndex) + ")"));

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
}

QList<AlterPlayFrame> AlterPlayFrame::readFrom(const QString &filepath)
{
    QList<AlterPlayFrame> playlist;
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
    }
    //更新listwidget的内容
    while(!file.atEnd()) {
        QString line = file.readLine().trimmed();
        int index = line.indexOf(":");
        QString length = line.mid(0, index);
        QString url = line.mid(index + 1);
        if(length.isEmpty() || url.isEmpty())
            continue;

        AlterPlayFrame frame;
        frame.url = url.trimmed();
        frame.length = length.trimmed().toInt();

        playlist << frame;
    }
    return playlist;
}
