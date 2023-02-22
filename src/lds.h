#ifndef LDS_H
#define LDS_H

#include <QRect>
#include <QWidget>
#include <QChar>
#include <QPixmap>

#define b40 40

/**
 * @brief The AlterPlayFrame struct
 * 轮播的播放信息
 */
struct AlterPlayFrame {
    QString url;
    int timeout;

    static QList<AlterPlayFrame> readFrom(const QString &filepath);
};

/**
 * @brief The lds class
 * 通用函数整合
 */
class lds : public QWidget
{
    Q_OBJECT
public:
    static const int margin;
    static const int marginX2;
    static const int border_width;
    static const int videoAreaRight;
    static const int videoAreaBottom;
public:
    lds(QWidget *parent = 0);

    static void init();

//    static QRect AppBoundingRect;
    static QRect moveToCenter(QWidget *widget, QRect rect);

    static QString iconFontFamily;
    static QPixmap getFontPixmap(QChar ch, const QColor &color = QColor("#f6f5fb"), const QSize &size = QSize(40, 40));

    static QPixmap getLayoutPixmap(int count);

    static QString getUniqueFileNamehByDateTime(const QString &dir);

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
