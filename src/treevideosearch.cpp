#include "treevideosearch.h"
#include "treevideoview.h"
#include "treevideomodel.h"
#include "json/json.h"
#include "lds.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QHeaderView>
#include <QDialog>

QPixmap getCameraStatePixmap(int state) {
    if(0 == state) {
        return lds::getFontPixmap(0x25cf, QColor("green"), QSize(12, 12));
    }

    if(1 == state) {
        return lds::getFontPixmap(0xf05e, QColor("red"), QSize(12, 12));

    }
    if(2 == state) {
        return lds::getFontPixmap(0x25cf, QColor(245, 180, 0), QSize(12, 12));
    }

    return QPixmap();
}

TreeVideoSearch::TreeVideoSearch(QWidget *parent)
    : QWidget(parent)
{
    QLabel *label = new QLabel;
    m_buttonClose = new QPushButton;
    m_lineEdit = new QLineEdit;
    m_comboBox = new QComboBox;
    m_treeView = new TreeVideoView;
    m_treeView->setHeaderHidden(true);
    m_treeView->header()->setStretchLastSection(true);
    m_treeView->expandAll();
    m_treeView->setDragEnabled(true);
    m_treeView->setEditTriggers(QTreeView::NoEditTriggers);
    m_treeModel = new TreeVideoModel(this);
    m_treeView->setModel(m_treeModel);

    m_comboBox->setFixedSize(108, 30);
    m_comboBox->setProperty("outer_stylesheet", "search");
    label->setFixedSize(30, 30);
    label->setProperty("outer_stylesheet", "search");
    m_lineEdit->setProperty("outer_stylesheet", "search");
    m_lineEdit->setFixedHeight(30);
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_buttonClose->setFixedSize(30, 30);
    m_buttonClose->setProperty("outer_stylesheet", "button_xmark");
    m_buttonClose->hide();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_comboBox);
    hlayout->addWidget(label);
    hlayout->addWidget(m_lineEdit, 10);
    hlayout->addWidget(m_buttonClose);
    hlayout->setSpacing(0);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_treeView, 1);
    vlayout->setSpacing(0);
    vlayout->setMargin(0);

    setLayout(vlayout);

    connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotShowCloseButton(QString)));
    connect(m_buttonClose, SIGNAL(clicked()), m_lineEdit, SLOT(clear()));
    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSelectStation(int)));
    connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSearchCamera(QString)));
    connect(m_treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(slotUpdateAndExpandNode(QModelIndex)));
    connect(m_treeView, SIGNAL(signalRefresh()), this, SLOT(slotInitAll()));
    connect(m_treeView, SIGNAL(signalSettings()), this, SLOT(slotSettings()));
    connect(m_treeView, SIGNAL(signalExportJson()), this, SLOT(slotExportJson()));
    connect(m_treeView, SIGNAL(signalImportJson()), this, SLOT(slotImportJson()));
}

void TreeVideoSearch::setDataSource(DataSource *datasource)
{
    m_datasource = datasource;
}

DataSource *TreeVideoSearch::dataSource()
{
    return m_datasource;
}

QAbstractItemModel *TreeVideoSearch::model()
{
    return m_treeModel;
}

QModelIndex TreeVideoSearch::currentIndex()
{
    return m_treeView->currentIndex();
}

void TreeVideoSearch::hideMenu()
{
    m_treeView->hideMenu();
}

void TreeVideoSearch::slotInitAll()
{
    slotInitSql();
    slotInitControl();
}

void TreeVideoSearch::slotInitSql()
{
    QSqlQuery q;
    q.exec("delete from vw_location");
    q.exec("delete from vw_device");
    q.exec("delete from vw_device_state");
    for(DataSource::Location location : m_datasource->getLocationList()) {
        QString obid = location.obid;
        QString name = location.name;
        int count = location.camera_count;

        q.exec(QString("insert into vw_location(obid, name, type, state) values('%1', '%2', %3, %4)")
               .arg(obid)
               .arg(name)
               .arg(location.type)
               .arg(location.state));


        if(0 < count) {
            //占位用
            q.exec(QString("insert into vw_device(obid, name, location_obid, type, state, url) values('%1', '', '%2', 0, 0, '')")
                   .arg(obid + "001")
                   .arg(obid));
        }
    }
    for(DataSource::CameraState state : m_datasource->getCameraStateList()) {
        q.exec(QString("insert into vw_device_state(rank, name) values(%1, '%2')")
               .arg(state.rank)
               .arg(state.name));
    }
}

void TreeVideoSearch::slotInitControl()
{
    m_comboBox->clear();
    m_comboBox->addItem(QString::fromUtf8("所有"), "%");
    QSqlQuery query_location;
    query_location.exec(QString("select obid, name from vw_location"));
    while(query_location.next()) {
        QString obid = query_location.record().value("obid").toString();
        QString name = query_location.record().value("name").toString();
        m_comboBox->addItem(name, obid);
    }
    this->updateCameraTree();
    m_treeView->expand(m_treeModel->index(0, 0));
}

void TreeVideoSearch::slotSettings()
{
    QDialog d(this);
    d.setObjectName("Window");
    QVBoxLayout *vlayout = new QVBoxLayout;
    QLabel *label  = new QLabel;
    QModelIndex index = m_treeView->currentIndex();
    label->setText(index.data(VideoUrlRole).toString());

    settings_edit=new QLineEdit;
    settings_edit->setText(QString("%1,%2 %3x%4")
                           .arg(this->parentWidget()->parentWidget()->x())
                           .arg(this->parentWidget()->parentWidget()->y())
                           .arg(this->parentWidget()->parentWidget()->width())
                           .arg(this->parentWidget()->parentWidget()->height())
                           );
    QPushButton *b = new QPushButton;
    b->setText("AppTo");
    connect(b, SIGNAL(clicked()), this, SLOT(slotAppSettings()));

    vlayout->addWidget(label);
    vlayout->addWidget(settings_edit);
    vlayout->addWidget(b);
    d.setLayout(vlayout);

    d.exec();
}

void TreeVideoSearch::slotAppSettings()
{
    QString xy = settings_edit->text().split(" ").value(0);
    QString wh = settings_edit->text().split(" ").value(1);
    int w = wh.split("x").value(0).toInt();
    int h = wh.split("x").value(1).toInt();
    int x = xy.split(",").value(0).toInt();
    int y = xy.split(",").value(1).toInt();
    this->parentWidget()->parentWidget()->setGeometry(x, y, w, h);
}

void TreeVideoSearch::slotExportJson()
{
    QFile file("video.json");
    file.open(QFile::WriteOnly);
    file.write(m_datasource->toJson());
    file.close();
}

void TreeVideoSearch::slotImportJson()
{
    m_datasource->fromJson("video.json");
    slotInitAll();
}

void TreeVideoSearch::addToPlayThread(const QString &obid, const QString &url)
{

}

void TreeVideoSearch::slotShowCloseButton(const QString &text)
{
    m_buttonClose->setVisible(text.count() > 0);
}

void TreeVideoSearch::slotSelectStation(int index)
{
    QString location_obid = m_comboBox->itemData(index).toString();
    if("%" == location_obid) {
        m_treeView->scrollToTop();
        return;
    }

    updateCameraSqlAndItemListOnce(location_obid);

    int station_index = index - 1;
    QStandardItem *itemRoot     = m_treeModel->invisibleRootItem();
    QStandardItem *itemISCS     = itemRoot ? itemRoot->child(0) : 0;
    QStandardItem *itemStation  = itemISCS ? itemISCS->child(station_index) : 0;
    if(!itemStation)
        return;

    m_treeView->setCurrentIndex(itemStation->index());
    m_treeView->expand(itemStation->index());
    m_treeView->scrollTo(itemStation->index(), QTreeView::PositionAtTop);
}

void TreeVideoSearch::slotSearchCamera(const QString &text)
{
    int camera_index = -1;
    int station_index = m_comboBox->currentIndex();
    QString obid = m_comboBox->itemData(station_index).toString();
    QSqlQuery query_location;
    QSqlQuery query_camera;
    QString location_obid;

    //移除 "所有" combobox item
    if("%" != obid)
        station_index --;
    query_location.exec(QString("select obid from vw_location where obid like '%1' ").arg(obid));
    while(query_location.next()) {

        location_obid = query_location.record().value("obid").toString();
        updateCameraSqlAndItemListOnce(location_obid);

        query_camera.exec(QString("select obid, name, location_obid from vw_device where location_obid = '%1' ")
                          .arg(location_obid));
        while(query_camera.next()) {
            if(query_camera.record().value("name").toString().contains(text, Qt::CaseInsensitive)) {
                goto search_end;
            }
        }
    }

    //没有找到
    return;

search_end:
    //定位treeview的节点
    station_index = station_index + query_location.at();
    camera_index = query_camera.at();
    if(camera_index >= 0 && station_index >= 0) {
        QStandardItem *itemRoot     = m_treeModel->invisibleRootItem();
        QStandardItem *itemISCS     = itemRoot ? itemRoot->child(0) : 0;
        QStandardItem *itemStation  = itemISCS ? itemISCS->child(station_index) : 0;
        QStandardItem *itemCamera   = itemStation ? itemStation->child(camera_index) : 0;
        m_treeView->setCurrentIndex(itemCamera ? itemCamera->index() : QModelIndex());
    }
}

void TreeVideoSearch::updateCameraSqlAndItemListOnce(const QString &location_obid)
{
    if(1 == lds::selectValue("select state from vw_location where obid = '%1'", location_obid))
        return;

    updateCameraSqlList(location_obid);
    updateCameraItemList(location_obid);

    QSqlQuery q;
    q.exec(QString("update vw_location set state = 1 where obid = '%1'").arg(location_obid));
}

void TreeVideoSearch::updateCameraSqlList(const QString &location_obid)
{
    QSqlQuery q;
    q.exec(QString("delete from vw_device where location_obid = '%1' ").arg(location_obid));
    for(DataSource::Camera c : m_datasource->getCameraList(location_obid)) {
        q.exec(QString("insert into vw_device values('%1', '%2', '%3', 1, %4, '%5')")
               .arg(c.obid)
               .arg(c.name)
               .arg(location_obid)
               .arg(c.state)
               .arg(c.url));
    }
}

void TreeVideoSearch::updateCameraTree()
{
    m_treeModel->clear();

    QStandardItem *itemRoot =  m_treeModel->invisibleRootItem();
    QStandardItem *itemISCS = new QStandardItem(QString::fromUtf8("摄像头"));
    itemRoot->appendRow(itemISCS);
    QSqlQuery query_location;
    query_location.exec("select obid, name from vw_location");
    while(query_location.next()) {
        QStandardItem *item_location = new QStandardItem;
        QString location_obid = query_location.record().value("obid").toString();
        QString location_name = query_location.record().value("name").toString();
        item_location->setText(location_name);
        item_location->setData(VideoNodeStation, VideoNodeType);
        item_location->setData(location_obid,    VideoObidRole);
        itemISCS->appendRow(item_location);

        updateCameraItemList(item_location);
    }
}

void TreeVideoSearch::updateCameraItemList(const QString &location_obid)
{
    QStandardItem *itemRoot =  m_treeModel->invisibleRootItem();
    QStandardItem *item =  itemRoot->child(0);
    QStandardItem *item_location = 0;
    for(int k = 0; item && k < item->rowCount(); k ++) {
        item_location = item->child(k);
        if(item_location && item_location->data(VideoObidRole).toString() == location_obid) {
            break;
        }
    }
    if(!item_location)
        return;
    updateCameraItemList(item_location);
}

void TreeVideoSearch::updateCameraItemList(QStandardItem *item_location)
{
    item_location->removeRows(0, item_location->rowCount());
    QString location_obid = item_location->data(VideoObidRole).toString();
    QSqlQuery query_device;

    query_device.exec(QString("select * from vw_device where location_obid = '%1' ").arg(location_obid));
    while(query_device.next()) {
        QString obid = query_device.record().value("obid").toString();
        QString name = QString::fromUtf8(query_device.record().value("name").toByteArray());
        int state = query_device.record().value("state").toInt();
        int type = query_device.record().value("type").toInt();
        QString url = query_device.record().value("url").toString();

        QString stateName = m_datasource->getCameraStateName(state);
        QString typeName = m_datasource->getCameraTypeName(type);

        QStandardItem *item_device = new QStandardItem;
        item_device->setText(name);
        item_device->setToolTip("[" + stateName+ "]" + "[" + typeName + "]" + name);
        item_device->setData(VideoNodeTrain,    VideoNodeType);
        item_device->setData(name,              VideoNameRole);
        item_device->setData(type,              VideoTypeRole);
        item_device->setData(obid,              VideoObidRole);
        item_device->setData(state,             VideoStateRole);
        item_device->setData(url,               VideoUrlRole);
        item_device->setData(getCameraStatePixmap(state),   Qt::DecorationRole);

        item_location->appendRow(item_device);

        //
        addToPlayThread(obid, url);
    }
}

void TreeVideoSearch::slotUpdateAndExpandNode(const QModelIndex &index)
{
    if(index.data(VideoNodeType).toInt() == VideoNodeStation) {
        QString obid = index.data(VideoObidRole).toString();
        updateCameraSqlAndItemListOnce(obid);
    }
}
