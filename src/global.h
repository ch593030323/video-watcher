#ifndef GLOBAL_H
#define GLOBAL_H

//树状图的子节点
#define VideoMinRole            Qt::UserRole + 1
#define VideoNameRole           Qt::UserRole + 1
#define VideoTypeRole           Qt::UserRole + 2
#define VideoObidRole           Qt::UserRole + 3
#define VideoStateRole          Qt::UserRole + 4
#define VideoUrlRole            Qt::UserRole + 5
#define VideoNodeTypeRole       Qt::UserRole + 6 //1:station, 2:train
#define VideoTypeNameRole       Qt::UserRole + 7
#define VideoStateNameRole      Qt::UserRole + 8
#define VideoMaxRole            Qt::UserRole + 9

#define VideoRoleMap            QMap<int, QVariant>
//树状图的节点
#define NodeTypeStation 1
#define NodeTypeDevice  2

//轮播
#define AlterTypeRole           Qt::UserRole + 10
#define AlterValueRole          Qt::UserRole + 11

#define FilePathRole            Qt::UserRole + 12


class global
{
public:
    global();
};

#endif // GLOBAL_H
