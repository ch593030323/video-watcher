#include "treevideoviewsearchsettings.h"
#include "treevideomodel.h"
#include "treevideoview.h"
#include "lds.h"

TreeVideoViewSearchSettings::TreeVideoViewSearchSettings(QWidget *parent)
    : TreeVideoViewSearch(parent)
{
    connect(m_treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(slotEditUrl(QModelIndex)));
}

void TreeVideoViewSearchSettings::addToPlayThread(const QString &url)
{

}

void TreeVideoViewSearchSettings::initTree()
{
    TreeVideoViewSearch::initTree();
    m_treeView->setColumnWidth(0, 200);
    m_treeView->slotExpandAll();
}

void TreeVideoViewSearchSettings::appendHeaderHorizontalItem(QStandardItem *itemRoot)
{
    itemRoot->setChild(0, 1, createItem(QString::fromUtf8("Url"), Qt::ItemIsEnabled | Qt::ItemIsSelectable));
}

void TreeVideoViewSearchSettings::appendDeviceHorizontalItem(QStandardItem *item_location, int row, QStandardItem *item_device)
{
    item_location->setChild(row, 1, new QStandardItem(item_device->data(VideoUrlRole).toString()));
}

void TreeVideoViewSearchSettings::slotEditUrl(const QModelIndex &index)
{
    if(VideoNodeTrain == index.data(VideoNodeType).toInt()) {
        //默认，第0列显示设备名，第1列是显示url
        m_treeView->edit(index.sibling(index.row(), 1));
    }
}
