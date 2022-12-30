#ifndef LDS_H
#define LDS_H


#include "ffmpeg.h"

#include <QRect>
#include <QWidget>
#include <QChar>
#include <QPixmap>
#include <QtSql>

#define b40 40
class VideoWidget;

/**
 * @brief The PlayThread class
 * 在原有播放线程的基础上加了些优化函数
 */
class PlayThread : public FFmpegThread{
    Q_OBJECT
public:
    explicit PlayThread(const QString &url, QObject *parent = 0) : FFmpegThread(url, parent) {}
    virtual ~PlayThread();
    int receiverImageConnectionCount() const;
    void open();
    void close();
};


/**
 * @brief The LayoutPos struct
 * 最大支持 100*100的棋盘
 */
struct LayoutPos{
    LayoutPos(){}
    LayoutPos(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int x;
    int y;

    int value() const {
        return x * 100 + y;
    }
    /**
     * @brief operator <
     * QMap会用到
     */
    bool operator <(const LayoutPos & p) const{
        return value() < p.value();
    }
};

/**
 * @brief The LayoutCell struct
 * 布局中一个单元格的info
 */
struct LayoutCell{
    LayoutCell(){
        pos.x = 0;
        pos.y = 0;
        row_spans = 1;
        column_spans = 1;
    }

    LayoutPos pos;
    int row_spans;
    int column_spans;
    QString device_obid;

    QString url() const;
    static QString url(QString device_obid);

    static int indexOf(QRect rect, const QList<LayoutCell> &list);

    QRect rect() const;

    inline int x() const {return pos.x;}
    inline int y() const {return pos.y;}
};

/**
 * @brief The LayoutInfo struct
 * 一个布局的info
 */
struct LayoutInfo{
    int column_count;
    int row_count;
    QList<LayoutCell> cells;

    LayoutInfo(){}
    LayoutInfo(int c, int r) {
         column_count = c;
         row_count = r;
    }
    QByteArray toJson();
    static LayoutInfo readFrom(const QByteArray &json);

    void update(const QMap<LayoutPos, VideoWidget *> &map);
};

/**
 * @brief The AlterPlayFrame struct
 * 轮播的播放信息
 */
struct AlterPlayFrame {
    QString device_obid;
    int timeout;

    static QList<AlterPlayFrame> readFrom(const QString &filepath);
};

class DataSource : public QObject{
    Q_OBJECT
public:
    struct Location {
        //for sql
        QString obid;
        QString name;
        int type;//车站的类型，0：车站、1：列车

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
    DataSource(QObject *parent = 0);

    virtual QList<Location> getLocationList();
    virtual QList<Camera> getCameraList(const QString &location_obid);
};

/**
 * @brief The lds class
 * 通用函数整合
 */
class lds : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor highlightColor READ gethighlightColor WRITE sethighlightColor DESIGNABLE true)
    Q_PROPERTY(QColor textColor      READ gettextColor      WRITE settextColor      DESIGNABLE true)
    Q_PROPERTY(QColor subwindowColor READ getsubwindowColor WRITE setsubwindowColor DESIGNABLE true)
public:
    void sethighlightColor(const QColor &color);
    QColor gethighlightColor() const;
    static QColor highlightColor;

    void settextColor(const QColor &color);
    QColor gettextColor() const;
    static QColor textColor;

    void setsubwindowColor(const QColor &color);
    QColor getsubwindowColor() const;
    static QColor subwindowColor;

    static QVariant selectValue(const QString &sql, const QString &arg0);

    static const int margin;
    static const int marginX2;
    static const int border_width;
    static const int videoAreaRight;
    static const int videoAreaBottom;
public:
    lds(QWidget *parent = 0);

    static void init();

//    static QRect AppBoundingRect;
    static QRect MoveToCenter(QWidget *widget, QRect rect);

    //url and play thread
    static QMap<QString, PlayThread *> PlayThreadMap;

    static QString IconFontFamily;
    static QPixmap getFontPixmap(QChar ch, const QColor &color = QColor("#f6f5fb"), const QSize &size = QSize(40, 40));

    static QPixmap getLayoutPixmap(int count);

    static QString styleSheetString;
};

/* 网卡如何追加ip地址
1.修改配置
vi /etc/network/interfaces

# interfaces(5) file used by ifup(8) and ifdown(8)
auto lo
iface lo inet loopback
auto ens16
iface ens16 inet static
address 10.2.1.3
netmask 255.255.255.0
up route add -net 234.0.0.0 netmask 255.255.255.0 dev ens16
#gateway 10.2.1.254

auto ens17
iface ens17 inet static
address 10.137.32.159
netmask 255.255.255.0

添加：auto ens17:0
添加：iface ens17:0 inet static
添加：address 192.7.3.100
添加：netmask 255.255.255.0

#up route add -net 234.0.1.0 netmask 255.255.255.0 dev ens17
#up route add -net 10.102.0.0 netmask 255.255.0.0 gw 10.102.1.254
gateway 10.137.32.254
dns-nameservers 172.18.7.24 172.20.201.110


2.重启网络
sudo /etc/init.d/networking restart
*/

#endif // LDS_H
