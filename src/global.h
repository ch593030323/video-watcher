#ifndef GLOBAL_H
#define GLOBAL_H

//树状图的子节点
#define VideoNameRole           Qt::UserRole + 1
#define VideoTypeRole           Qt::UserRole + 2
#define VideoObidRole           Qt::UserRole + 3
#define VideoStateRole          Qt::UserRole + 4
#define VideoUrlRole            Qt::UserRole + 5
#define VideoNodeType           Qt::UserRole + 6 //1:station, 2:train

//轮播
#define AlterTypeRole           Qt::UserRole + 7
#define AlterValueRole          Qt::UserRole + 8

#define FilePathRole            Qt::UserRole + 9

//树状图的子节点
//名称
#define VideoTypeNameRole       Qt::UserRole + 10
#define VideoStateNameRole      Qt::UserRole + 11

//树状图的节点
#define VideoNodeStation 1
#define VideoNodeDevice  2

class global
{
public:
    global();
};

#endif // GLOBAL_H
