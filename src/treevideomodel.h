#ifndef TREEVIDEOMODEL_H
#define TREEVIDEOMODEL_H

#include <QStandardItemModel>
//树状图的子节点
#define VideoNameRole    Qt::UserRole + 1
#define VideoTypeRole    Qt::UserRole + 2
#define VideoObidRole    Qt::UserRole + 3
#define VideoStateRole   Qt::UserRole + 4
#define VideoUrlRole     Qt::UserRole + 5
#define VideoNodeType    Qt::UserRole + 6 //1:station, 2:train

//轮播
#define AlterTypeRole    Qt::UserRole + 7
#define AlterValueRole   Qt::UserRole + 8

//树状图的节点
#define VideoNodeStation 1
#define VideoNodeTrain   2

class TreeVideoModel : public QStandardItemModel
{
public:
    TreeVideoModel(QObject *parent = 0);

    QMimeData*  mimeData(const QModelIndexList &indexes) const;
};

#endif // TREEVIDEOMODEL_H
