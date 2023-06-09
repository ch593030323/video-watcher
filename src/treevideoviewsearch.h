#ifndef TREEVIDEOVIEWSEARCH_H
#define TREEVIDEOVIEWSEARCH_H

#include <QWidget>
#include <QStandardItem>

#include "datasource.h"

class TreeVideoView;
class QAbstractItemModel;
class QComboBox;
class QLineEdit;
class QPushButton;
class TreeVideoModel;
class QTreeView;
class LineEditFind;

/**
 * @brief The TreeVideoViewSearch class
 * 支持多数据源搜索的树形控件
 */
class TreeVideoViewSearch : public QWidget
{
    Q_OBJECT
public:
    enum MateType{
        StartsWith,
        EndsWidth,
        Contains,
        Equal
    };
    bool mateString(const QString &lvalue, const QString &rvalue, MateType type);

public:
    TreeVideoViewSearch(QWidget *parent = 0);

    void setDataSource(DataSource *datasource);
    DataSource *dataSource();

    QAbstractItemModel *model();
    TreeVideoView *view();
    QModelIndex currentIndex();

    void updateItemTipText(QStandardItem *item);

    QModelIndex findItem(const QString &text, int role, MateType type);

public slots:
    void slotInitAll();
    void slotInitTree();
    void slotSettings();
    void slotAppSettings();
    void slotExportJson();
    void slotImportJson();

protected:
    virtual void addToPlayThread(const QString &url);
    virtual void initTree();
    virtual void appendHeaderHorizontalItem(QStandardItem *itemRoot);
    virtual void appendDeviceHorizontalItem(QStandardItem *item_location, int row, QStandardItem *item_device);

protected slots:
    void slotSelectStation(int index);
    void slotSearchCamera(const QString &text);
    void slotUpdateAndExpandNode(const QModelIndex &index);
    void slotSetColor();

protected:
    void updateCameraItemListOnce(const QString &location_obid);
    void updateCameraItemList(QStandardItem *item_location);
    void updateCameraItemList(const QString &location_obid);

    QStandardItem *createItem(const QString &text, Qt::ItemFlags flags);
    QStandardItem *getLocationItem(const QString &location_obid);
public:
    TreeVideoView *m_treeView;
    QComboBox *m_comboBox;
    LineEditFind *m_lineEdit;
    DataSource *m_datasource;
    TreeVideoModel *m_treeModel;
};

#endif // TREEVIDEOVIEWSEARCH_H
