#ifndef TREEVIDEOVIEWSEARCHSETTINGS_H
#define TREEVIDEOVIEWSEARCHSETTINGS_H

#include "treevideoviewsearch.h"

#include <QItemSelection>
#include <QAbstractItemDelegate>

class TreeVideoViewSearchSettings : public TreeVideoViewSearch
{
    Q_OBJECT
public:
    TreeVideoViewSearchSettings(QWidget *parent = 0);

signals:
    void editingFinished();

public slots:
    void slotEditUrl(const QModelIndex &index);
    void slotEditUrl(const QItemSelection &selected, const QItemSelection &deselected);

    void slotSetModelDataByEdit();

protected:
    virtual void addToPlayThread(const QString &url);
    virtual void initTree();
    virtual void appendHeaderHorizontalItem(QStandardItem *itemRoot);
    virtual void appendDeviceHorizontalItem(QStandardItem *item_location, int row, QStandardItem *item_device);
};

#endif // TREEVIDEOVIEWSEARCHSETTINGS_H
