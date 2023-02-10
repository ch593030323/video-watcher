#include "treevideoviewsearch.h"
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
#include <QLineEdit>
#include <QColorDialog>
#include <QApplication>
#include <QtDebug>

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

TreeVideoViewSearch::TreeVideoViewSearch(QWidget *parent)
    : QWidget(parent)
    , m_isShowUrlColumn(false)
{
    QLabel *label = new QLabel;
    m_buttonClose = new QPushButton;
    m_lineEdit = new QLineEdit;
    m_comboBox = new QComboBox;

    m_treeModel = new TreeVideoModel(this);
    m_treeView = new TreeVideoView;
    m_treeView->setHeaderHidden(true);
    m_treeView->header()->setStretchLastSection(true);
    m_treeView->setDragEnabled(true);
    m_treeView->setEditTriggers(QTreeView::AllEditTriggers);
    m_treeView->setModel(m_treeModel);
    m_treeView->setDragEnabled(true);

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
    connect(m_treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(slotEditUrl(QModelIndex)));
}

void TreeVideoViewSearch::setDataSource(DataSource *datasource)
{
    m_datasource = datasource;
}

DataSource *TreeVideoViewSearch::dataSource()
{
    return m_datasource;
}

QAbstractItemModel *TreeVideoViewSearch::model()
{
    return m_treeModel;
}

TreeVideoView *TreeVideoViewSearch::view()
{
    return m_treeView;
}

QModelIndex TreeVideoViewSearch::currentIndex()
{
    return m_treeView->currentIndex();
}

void TreeVideoViewSearch::hideMenu()
{
    m_treeView->hideMenu();
}

void TreeVideoViewSearch::setShowUrlColumn(bool isShowUrlColumn)
{
    m_isShowUrlColumn = isShowUrlColumn;
}

void TreeVideoViewSearch::slotInitAll()
{
    slotInitSql();
    slotInitControl();

    if(m_isShowUrlColumn) {
        m_treeView->setColumnWidth(0, 200);
        m_treeView->slotExpandAll();
    }
}

void TreeVideoViewSearch::slotInitSql()
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
                   .arg("")
                   .arg(obid));
        }
    }
    for(DataSource::CameraState state : m_datasource->getCameraStateList()) {
        q.exec(QString("insert into vw_device_state(rank, name) values(%1, '%2')")
               .arg(state.rank)
               .arg(state.name));
    }
}

void TreeVideoViewSearch::slotInitControl()
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

void TreeVideoViewSearch::slotSettings()
{
    QDialog d(this);
    d.setObjectName("Window");
    QGridLayout *vlayout = new QGridLayout;
    QFile file("skin.qss");
    file.open(QFile::ReadOnly);
    int row = 0;
    while(!file.atEnd()) {
        QString line = file.readLine();
        int index = line.indexOf("qproperty");
        if(index >= 0) {
            index = line.indexOf("-");
            int index2 = line.indexOf(":");
            QString name = line.mid(index + 1, index2 - index - 1);
            QString color = line.mid(index2 + 1).trimmed();
            color.chop(1);

            QLabel *label = new QLabel;
            QPushButton *button = new QPushButton;
            button->setMinimumHeight(40);

            label->setText(name);
            button->setText(color);
            vlayout->addWidget(label, row, 0);
            vlayout->addWidget(button, row, 1);
            row ++;
            connect(button, SIGNAL(clicked()), this, SLOT(slotSetColor()));
        }
    }
    QPushButton *acceptButton = new QPushButton("OK");
    acceptButton->setFixedHeight(40);
    QPushButton *rejectButton = new QPushButton("Exit");
    rejectButton->setFixedHeight(40);
    vlayout->addWidget(acceptButton, row, 0);
    vlayout->addWidget(rejectButton, row, 1);
    connect(acceptButton, SIGNAL(clicked()), this, SLOT(slotAppSettings()));
    connect(rejectButton, SIGNAL(clicked()), &d, SLOT(reject()));

    d.setLayout(vlayout);
    d.resize(500, 400);

    d.exec();
}

void TreeVideoViewSearch::slotAppSettings()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    QObject *w = button->parent();
    QFile file("skin.qss");
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();

    for(QObject *o : w->children())
    {
        QString text = o->property("text").toString();
        if(text.startsWith("#") && text.contains("->")) {
            QString l = text.split("->").value(0);
            QString r = text.split("->").value(1);
            all.replace(l, r.toLocal8Bit());
            qDebug() << l << r;
        }
    }
    file.close();
    file.open(QFile::WriteOnly);
    file.write(all);
    this->setStyleSheet(QString() + all);
    this->update();
}

void TreeVideoViewSearch::slotExportJson()
{
    QFile file("video.json");
    file.open(QFile::WriteOnly);
    file.write(m_datasource->toJson());
    file.close();
}

void TreeVideoViewSearch::slotImportJson()
{
    m_datasource->fromJson("video.json");
    slotInitAll();
}

void TreeVideoViewSearch::slotEditUrl(const QModelIndex &index)
{
    if(m_isShowUrlColumn) {
        if(VideoNodeTrain == index.data(VideoNodeType).toInt()) {
            //默认，第0列显示设备名，第1列是显示url
            m_treeView->edit(index.sibling(index.row(), 1));
        }
    }
}

void TreeVideoViewSearch::addToPlayThread(const QString &obid, const QString &url)
{

}

void TreeVideoViewSearch::slotShowCloseButton(const QString &text)
{
    m_buttonClose->setVisible(text.count() > 0);
}

void TreeVideoViewSearch::slotSelectStation(int index)
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

void TreeVideoViewSearch::slotSearchCamera(const QString &text)
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

void TreeVideoViewSearch::updateCameraSqlAndItemListOnce(const QString &location_obid)
{
    QStandardItem *item = getLocationItem(location_obid);
    //若第一个设备的obid为空，则需要加载数据
    if(item && item->child(0) && item->child(0)->data(VideoObidRole).toString().isEmpty()) {
        updateCameraSqlList(location_obid);
        updateCameraItemList(location_obid);
    }
}

void TreeVideoViewSearch::updateCameraSqlList(const QString &location_obid)
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

void TreeVideoViewSearch::updateCameraTree()
{
    m_treeModel->clear();

    QStandardItem *itemRoot =  m_treeModel->invisibleRootItem();

    QStandardItem *itemISCS = createItem(QString::fromUtf8("摄像头"), Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    itemRoot->setChild(0, 0, itemISCS);
    if(m_isShowUrlColumn)
        itemRoot->setChild(0, 1, createItem(QString::fromUtf8("Url"), Qt::ItemIsEnabled | Qt::ItemIsSelectable));

    QSqlQuery query_location;
    query_location.exec("select obid, name from vw_location");
    while(query_location.next()) {
        QStandardItem *item_location = new QStandardItem;
        QString location_obid = query_location.record().value("obid").toString();
        QString location_name = query_location.record().value("name").toString();
        item_location->setText(location_name);
        item_location->setData(VideoNodeStation, VideoNodeType);
        item_location->setData(location_obid,    VideoObidRole);
        item_location->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemISCS->appendRow(item_location);

        updateCameraItemList(item_location);
    }
}

QStandardItem *TreeVideoViewSearch::createItem(const QString &text, Qt::ItemFlags flags)
{
    QStandardItem *item = new QStandardItem(text);
    item->setFlags(flags);
    return item;
}

QStandardItem *TreeVideoViewSearch::getLocationItem(const QString &location_obid)
{
    QStandardItem *itemRoot =  m_treeModel->invisibleRootItem();
    QStandardItem *item =  itemRoot->child(0);
    QStandardItem *item_location = 0;
    for(int k = 0; item && k < item->rowCount(); k ++) {
        item_location = item->child(k);
        if(item_location && item_location->data(VideoObidRole).toString() == location_obid) {
            return item_location;
        }
    }
    return 0;
}

void TreeVideoViewSearch::updateCameraItemList(const QString &location_obid)
{
    QStandardItem *item_location = getLocationItem(location_obid);
    if(!item_location)
        return;
    updateCameraItemList(item_location);
}

void TreeVideoViewSearch::updateCameraItemList(QStandardItem *item_location)
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

        int row = item_location->rowCount();
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
        item_device->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

        item_location->setChild(row, 0, item_device);
        if(m_isShowUrlColumn)
            item_location->setChild(row, 1, new QStandardItem(url));

        //
        addToPlayThread(obid, url);
    }
}

void TreeVideoViewSearch::slotUpdateAndExpandNode(const QModelIndex &index)
{
    if(index.data(VideoNodeType).toInt() == VideoNodeStation) {
        QString obid = index.data(VideoObidRole).toString();
        updateCameraSqlAndItemListOnce(obid);
    }
}

void TreeVideoViewSearch::slotSetColor()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    QColorDialog dialog;
    QString colorName = button->text();
    QString colorOrg = colorName;
    QString colorCur = colorOrg;
    if(colorName.contains("->"))  {
        colorOrg = colorName.split("->").value(0);
        colorCur = colorName.split("->").value(1);
    }
    dialog.setCurrentColor(QColor(colorCur));
    if(QDialog::Accepted ==  dialog.exec()) {
        button->setText(colorOrg + "->" + dialog.currentColor().name());
    }
}
