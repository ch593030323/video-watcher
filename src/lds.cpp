#include "lds.h"
#include "videowidget.h"
#include "json/json.h"

//QRect lds::AppBoundingRect;
QMap<QString, PlayThread *> lds::PlayThreadMap;
QString lds::IconFontFamily;

QColor lds::highlightColor;
QColor lds::textColor;
QColor lds::subwindowColor;

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

void lds::setsubwindowColor(const QColor &color)
{
    subwindowColor = color;
}

QColor lds::getsubwindowColor() const
{
    return subwindowColor;
}

QVariant lds::selectValue(const QString &sql, const QString &arg0)
{
    QSqlQuery query;
    query.exec(sql.arg(arg0));
    query.next();
    return query.record().value(0);
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
    QFile file(":/skin.qss");
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
    qRegisterMetaType<FFmpegData>("FFmpegData");
}

QString LayoutCell::url() const
{
    return url(device_obid);
}

QString LayoutCell::url(QString device_obid)
{
    QSqlQuery query_device;
    query_device.exec(QString(" select url from wx_device where obid = '%1'").arg(device_obid));
    query_device.next();
    return query_device.record().value("url").toString();
}

int LayoutCell::indexOf(QRect rect, const QList<LayoutCell> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        if(list[k].rect() == rect)
            return k;
    }
    return -1;
}

QRect LayoutCell::rect() const
{
    return QRect(pos.x, pos.y, column_spans, row_spans);
}

QByteArray LayoutInfo::toJson()
{
    Json::Value root;
    Json::FastWriter writer;

    root["column_count"] = column_count;
    root["row_count"] = row_count;

    Json::Value json_cells;
    for(int k = 0; k < cells.count(); k ++) {
        Json::Value json_cell;
        json_cell["x"] = cells[k].x();
        json_cell["y"] = cells[k].y();
        json_cell["column_spans"] = cells[k].column_spans;
        json_cell["row_spans"] = cells[k].row_spans;
        json_cell["device_obid"] = cells[k].device_obid.toStdString();

        json_cells.append(json_cell);
    }
    root["cells"] = json_cells;

    std::string json_file = writer.write(root);

    return QString::fromStdString(json_file).toLocal8Bit();
}

LayoutInfo LayoutInfo::readFrom(const QByteArray &json)
{
    LayoutInfo info;
    Json::Value value;
    Json::Reader reader;
    if(!reader.parse(json.begin(), json.end(), value)) {
        qDebug() << "json formate is error";
    }
    info.column_count = value["column_count"].asInt();
    info.row_count = value["row_count"].asInt();

    Json::Value cells = value["cells"];
    for(int k = 0; k < cells.size(); k ++) {
        LayoutCell cell;
        cell.pos.x = cells[k]["x"].asInt();
        cell.pos.y = cells[k]["y"].asInt();
        cell.column_spans   = cells[k]["column_spans"].asInt();
        cell.row_spans      = cells[k]["row_spans"].asInt();
        cell.device_obid    = cells[k]["device_obid"].asCString();

        info.cells.append(cell);
    }
    return info;
}

void LayoutInfo::update(const QMap<LayoutPos, VideoWidget *> &map)
{
    //将videoWidget的摄像头id保存到layoutinfo里
    for(QMap<LayoutPos, VideoWidget *>::const_iterator k = map.begin(); k != map.end(); k ++) {
        VideoWidget *w = k.value();
        QString obid = w->device_obid();
        if(w->isVisible() && obid != "") {
            QList<LayoutCell> &cells = this->cells;
            int index = LayoutCell::indexOf(w->rectX(), cells);
            if(index >= 0) {
                cells[index].device_obid = obid;
            } else {
                cells << w->getInfo();
            }
        }
    }
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

PlayThread::~PlayThread()
{
    if (this->isRunning()) {
        this->stop();
        this->quit();
        this->wait();
    }
}

int PlayThread::receiverImageConnectionCount() const
{
    return QObject::receivers(SIGNAL(receiveImage(FFmpegData)));
}

void PlayThread::open()
{
    if(isRunning()) {
        FFmpegThread::play();
    } else {
        FFmpegThread::start();
    }
}

void PlayThread::close()
{
    this->stop();
}

DataSource::DataSource(QObject *parent)
    : QObject(parent)
{

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
