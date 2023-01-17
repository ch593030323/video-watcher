#ifndef TREEVIDEOSEARCH_H
#define TREEVIDEOSEARCH_H

#include <QWidget>
#include <QStandardItem>

#include "datasource.h"

class TreeVideoView;
class QAbstractItemModel;
class QComboBox;
class QLineEdit;
class QPushButton;
class TreeVideoModel;

/**
 * @brief The TreeVideoSearch class
 * 支持多数据源搜索的树形控件
 */
class TreeVideoSearch : public QWidget
{
    Q_OBJECT
public:
    TreeVideoSearch(QWidget *parent = 0);

    void setDataSource(DataSource *datasource);
    DataSource *dataSource();

    QAbstractItemModel *model();

    QModelIndex currentIndex();

    void hideMenu();

public slots:
    void slotInitAll();
    void slotInitSql();
    void slotInitControl();
    void slotSettings();
    void slotAppSettings();
    void slotExportJson();
    void slotImportJson();

protected:
    virtual void addToPlayThread(const QString &obid, const QString &url);

private slots:
    void slotShowCloseButton(const QString &text);
    void slotSelectStation(int index);
    void slotSearchCamera(const QString &text);
    void slotUpdateAndExpandNode(const QModelIndex &index);

private:
    void updateCameraSqlAndItemListOnce(const QString &location_obid);
    void updateCameraSqlList(const QString &location_obid);
    void updateCameraItemList(QStandardItem *item_location);
    void updateCameraItemList(const QString &location_obid);
    void updateCameraTree();

public:
    TreeVideoView *m_treeView;
    QComboBox *m_comboBox;
    QLineEdit *m_lineEdit;
    QPushButton *m_buttonClose;
    DataSource *m_datasource;
    TreeVideoModel *m_treeModel;

    QLineEdit *settings_edit;
};

#endif // TREEVIDEOSEARCH_H
