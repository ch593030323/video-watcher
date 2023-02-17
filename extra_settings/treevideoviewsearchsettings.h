#ifndef TREEVIDEOVIEWSEARCHSETTINGS_H
#define TREEVIDEOVIEWSEARCHSETTINGS_H

#include "treevideoviewsearch.h"

class TreeVideoViewSearchSettings : public TreeVideoViewSearch
{
    Q_OBJECT
public:
    TreeVideoViewSearchSettings(QWidget *parent = 0);

public slots:
    void slotEditUrl(const QModelIndex &index);

protected:
    virtual void addToPlayThread(const QString &url);
    virtual void initTree();
    virtual void appendHeaderHorizontalItem(QStandardItem *itemRoot);
    virtual void appendDeviceHorizontalItem(QStandardItem *item_location, int row, QStandardItem *item_device);

};

#endif // TREEVIDEOVIEWSEARCHSETTINGS_H
