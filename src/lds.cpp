#include "lds.h"
#include "json/json.h"
#include "propertycolor.h"
#include "treevideoview.h"

#include <QFont>
#include <QApplication>
#include <QPainter>
#include <QFontDatabase>
#include <QtDebug>
#include <QFile>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QStatusBar>
#include <QProcessEnvironment>
#include <QMetaMethod>

const int lds::margin = 10;
const int lds::marginX2 = 2 * lds::margin;
const int lds::border_width = 2;
const int lds::videoAreaRight = 280;
const int lds::videoAreaBottom = 50;

QString lds::iconFontFamily;
QString lds::styleSheetString;
QString lds::configDirectory = ".";
DataSource *lds::dataSource = 0;
QStatusBar *lds::statusBar = 0;
TreeVideoSignalTransfer *lds::treeSignalTransfer = 0;
QString lds::windowName = "video-watcher";


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

QString lds::getUniqueFileNamehByDateTime(const QString &dir, const QString &prefix)
{
    //自动填充新增的文件名
    int maxIndex = -1;
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm");
    if(!prefix.isEmpty())
        fileName = prefix + "_" + fileName;

    for(const QFileInfo &info : QDir(dir).entryInfoList()) {
        if(info.isFile() && info.baseName().startsWith(fileName)) {
            int begin = info.baseName().lastIndexOf("(");
            int end = info.baseName().lastIndexOf(")");
            maxIndex = qMax(maxIndex, info.baseName().mid(begin + 1, end - begin - 1).toInt());
        }
    }
    maxIndex++;

    return (maxIndex == 0 ? fileName : (fileName + "(" + QString::number(maxIndex) + ")"));

}

QString lds::getUniqueFilePathhByDateTime(const QString &dir, const QString &prefix, const QString &suffix)
{
    QString name = getUniqueFileNamehByDateTime(dir, prefix);
    return dir + "/" + name + "." + suffix;
}

QString lds::getUniqueFileName(const QString &dir, const QString &name)
{
    //自动填充新增的文件名
    int maxIndex = -1;
    for(const QFileInfo &info : QDir(dir).entryInfoList()) {
        const QString baseName = info.completeBaseName();
        if(info.isFile() && baseName.startsWith(name)) {
            int begin = baseName.lastIndexOf("(");
            int end = baseName.lastIndexOf(")");
            //避免，baseName:11.png, name: 1的情况
            if(begin != -1 && name.size() != begin)
                continue;
            maxIndex = qMax(maxIndex, baseName.mid(begin + 1, end - begin - 1).toInt());
        }
    }
    maxIndex++;

    return (maxIndex == 0 ? name : (name + "(" + QString::number(maxIndex) + ")"));
}

QString lds::getuniqueFilePath(const QString &dir, const QString &baseName, const QString &suffix)
{
    QString name = getUniqueFileName(dir, baseName);
    return dir + "/" + name + "." + suffix;
}

void lds::showMessage(const QString &text)
{
    if(!statusBar)
        return;
    QMetaObject::invokeMethod(statusBar, "showMessage", Q_ARG(QString, QString::fromUtf8(qPrintable(text))));
}

void lds::showMessage(const QString &description, const QString &errstring)
{
    showMessage(description + ":" + errstring);
}

QList<QStandardItem *> lds::treeChildren(QStandardItem *node, int column)
{
    QList<QStandardItem *> itemList;
    for(int row = 0; row < node->rowCount(); row ++) {
        itemList << node->child(row, column);
    }
    return itemList;
}

void lds::init(QObject *parent)
{
    //text code
    //    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    //    QTextCodec::setCodecForLocale(codec);

    //#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    //#else
    //    QTextCodec::setCodecForCStrings(codec);
    //    QTextCodec::setCodecForTr(codec);
    //#endif

    //icon font
    int fontId = QFontDatabase::addApplicationFont(":/Awesome.otf");
    iconFontFamily = QFontDatabase::applicationFontFamilies(fontId).value(0);

    qDebug() << "iconFontFamily:" << iconFontFamily << QFontDatabase::applicationFontFamilies(fontId) << fontId;

    //skin
    QFile file(":/skin_light.qss");
    if(!file.open(QFile::ReadOnly)) {
        lds::showMessage((file.errorString() + ":" + file.fileName()));
    }

    styleSheetString = file.readAll();
    //    qApp->setStyleSheet(styleSheetString);

    PropertyColor::init(iconFontFamily, styleSheetString);

    //font
    //    QFont font = qApp->font();
    //    font.setPixelSize(25);
    //    qApp->setFont(font);


    //configDirectory
#ifdef OMS_DATASOURCE
    lds::configDirectory =  QProcessEnvironment::systemEnvironment().value("CPS_ENV") + "/data/video-watcher";
    QDir().mkpath(lds::configDirectory);
#else
    lds::configDirectory =  "../data/video-watcher";
    QDir().mkpath(lds::configDirectory);
#endif

    treeSignalTransfer = new TreeVideoSignalTransfer(parent);
}

QList<AlterPlayFrame> AlterPlayFrame::readFrom(const QString &filepath)
{
    QList<AlterPlayFrame> playlist;
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        lds::showMessage((file.errorString() + ":" + file.fileName()));
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
