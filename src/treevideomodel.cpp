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
        QByteArray url = index.data(VideoUrlRole).toByteArray();
        if(url != "")
            mimeD->setData("camera/url", index.data(VideoUrlRole).toByteArray());
    }

    return mimeD;
}
