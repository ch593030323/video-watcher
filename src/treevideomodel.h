#ifndef TREEVIDEOMODEL_H
#define TREEVIDEOMODEL_H

#include <QStandardItemModel>
#include "global.h"

class TreeVideoModel : public QStandardItemModel
{
public:
    TreeVideoModel(QObject *parent = 0);

    QMimeData*  mimeData(const QModelIndexList &indexes) const;
};

#endif // TREEVIDEOMODEL_H
