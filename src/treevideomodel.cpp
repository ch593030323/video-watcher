#include "treevideomodel.h"
#include <QMimeData>

TreeVideoModel::TreeVideoModel(QObject *parent)
    : QStandardItemModel(parent)
{

}

QMimeData *TreeVideoModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* mimeD = QStandardItemModel::mimeData(indexes);

    if (indexes.size() > 0) {
        QModelIndex index = indexes.at(0);
        QByteArray obid = index.data(VideoObidRole).toByteArray();
        if(obid != "")
            mimeD->setData("camera/obid", index.data(VideoObidRole).toByteArray());
    }

    return mimeD;
}
