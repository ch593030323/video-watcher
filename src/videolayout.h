#ifndef VIDEOLAYOUT_H
#define VIDEOLAYOUT_H

#include <QString>
#include <QRect>
#include <QList>
#include <QMap>

class VideoCell;

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
    QString url;

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
    LayoutInfo(int c) {
         column_count = c;
         row_count = c;
    }
    LayoutInfo(int c, int r) {
         column_count = c;
         row_count = r;
    }
    QByteArray toJson();
    static LayoutInfo readFrom(const QByteArray &json);

    void update(const QMap<LayoutPos, VideoCell *> &map);
};

#endif // VIDEOLAYOUT_H
