#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include "src/datasource.h"

#include <QFrame>
#include <QStandardItemModel>
#include <QAbstractItemDelegate>

namespace Ui {
class mainWidget;
}

class JsonDataSource : public DataSource {

    Q_OBJECT
public:
    JsonDataSource(const QString &jsonPath, QObject *parent = 0);

    void update();

    virtual QList<Location> getLocationList();
    virtual QList<Camera> getCameraList(const QString &location_obid);
    virtual QList<CameraState> getCameraStateList();

private:
    QString m_jsonPath;
    QList<Location> m_locationList;
    QList<Camera> m_cameraList;
    QList<CameraState> m_cameraStateList;
};

class mainWidget : public QFrame
{
    Q_OBJECT

public:
    explicit mainWidget(QWidget *parent = 0);
    ~mainWidget();

private slots:
    void slotUpdateInfo(const QModelIndex &current, const QModelIndex &previous);
    void toexit();
    void toreload();
    void tosub_defatult_video_url();
    void toadd_defatult_video_url();
    void toCheckTable(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);


private:
    Ui::mainWidget *ui;
    JsonDataSource *m_datasource;
    QStandardItemModel *m_tableModel;
};

#endif // MAINWIDGET_H
