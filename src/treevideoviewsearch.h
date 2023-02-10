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

/**
 * @brief The TreeVideoViewSearch class
 * 支持多数据源搜索的树形控件
 */
class TreeVideoViewSearch : public QWidget
{
    Q_OBJECT
public:
    TreeVideoViewSearch(QWidget *parent = 0);

    void setDataSource(DataSource *datasource);
    DataSource *dataSource();

    QAbstractItemModel *model();
    TreeVideoView *view();
    QModelIndex currentIndex();

    void hideMenu();

    void setShowUrlColumn(bool isShowUrlColumn);

public slots:
    void slotInitAll();
    void slotInitSql();
    void slotInitControl();
    void slotSettings();
    void slotAppSettings();
    void slotExportJson();
    void slotImportJson();
    void slotEditUrl(const QModelIndex &index);

protected:
    virtual void addToPlayThread(const QString &obid, const QString &url);

private slots:
    void slotShowCloseButton(const QString &text);
    void slotSelectStation(int index);
    void slotSearchCamera(const QString &text);
    void slotUpdateAndExpandNode(const QModelIndex &index);
    void slotSetColor();

private:
    void updateCameraSqlAndItemListOnce(const QString &location_obid);
    void updateCameraSqlList(const QString &location_obid);
    void updateCameraItemList(QStandardItem *item_location);
    void updateCameraItemList(const QString &location_obid);
    void updateCameraTree();

    QStandardItem *createItem(const QString &text, Qt::ItemFlags flags);
public:
    TreeVideoView *m_treeView;
    QComboBox *m_comboBox;
    QLineEdit *m_lineEdit;
    QPushButton *m_buttonClose;
    DataSource *m_datasource;
    TreeVideoModel *m_treeModel;

    QLineEdit *settings_edit;
    bool m_isShowUrlColumn;
};

#endif // TREEVIDEOVIEWSEARCH_H
