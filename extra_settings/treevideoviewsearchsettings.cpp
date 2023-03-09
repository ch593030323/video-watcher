#include "treevideoviewsearchsettings.h"
#include "treevideomodel.h"
#include "treevideoview.h"
#include "lds.h"

#include <QtDebug>

TreeVideoViewSearchSettings::TreeVideoViewSearchSettings(QWidget *parent)
    : TreeVideoViewSearch(parent)
{
    connect(m_treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(slotEditUrl(QItemSelection,QItemSelection)));
    connect(m_treeView->itemDelegate(), SIGNAL(closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint)),
            this, SLOT(slotSaveUrl(QWidget *, QAbstractItemDelegate::EndEditHint)));
    connect(m_treeView->itemDelegate(), SIGNAL(closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint)),
            this, SIGNAL(editingFinished()));

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
    item_device->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    QStandardItem *item = new QStandardItem(item_device->data(VideoUrlRole).toString());
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    item->setData(item_device->data(VideoObidRole), VideoObidRole);
    item_location->setChild(row, 1, item);
}

void TreeVideoViewSearchSettings::slotEditUrl(const QModelIndex &index)
{
    if(VideoNodeDevice == index.data(VideoNodeType).toInt()) {
        //默认，第0列显示设备名，第1列是显示url
        m_treeView->edit(index.sibling(index.row(), 1));
    }
}

void TreeVideoViewSearchSettings::slotEditUrl(const QItemSelection &selected, const QItemSelection &deselected)
{
    slotEditUrl(selected.indexes().value(0));
}

void TreeVideoViewSearchSettings::slotSaveUrl(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    if(editor) {
        QModelIndex index = m_treeView->indexAt(editor->geometry().center());
        if(index.isValid()) {
            m_datasource->cameraUrlCache.insert(index.data(VideoObidRole).toString(),
                                              editor->property("text").toString().trimmed());
        }
    }
}
