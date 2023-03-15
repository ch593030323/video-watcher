#include "treevideoviewsearch.h"
#include "treevideoview.h"
#include "treevideomodel.h"
#include "json/json.h"
#include "lds.h"
#include "lineeditfind.h"

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
#include <QFile>
#include <QListView>
#include <QTableView>

//1：在线、2：离线、3：故障
QPixmap getCameraStatePixmap(int state) {
    if(0 == state) {
        return lds::getFontPixmap(0x25cf, QColor("green"), QSize(12, 12));
    }

    if(1 == state) {
        return lds::getFontPixmap(0xd7, QColor("red"), QSize(12, 12));

    }
    if(2 == state) {
        return lds::getFontPixmap(0x25cf, QColor(245, 180, 0), QSize(12, 12));
    }

    return QPixmap();
}

TreeVideoViewSearch::TreeVideoViewSearch(QWidget *parent)
    : QWidget(parent)
{
    QTableView *comView = new QTableView;
    comView->horizontalHeader()->hide();
    comView->verticalHeader()->hide();
    comView->setEditTriggers(QTableView::NoEditTriggers);
    comView->verticalHeader()->setDefaultSectionSize(22);
    comView->horizontalHeader()->setStretchLastSection(true);
    comView->setShowGrid(false);

    m_lineEdit = new LineEditFind;
    m_comboBox = new QComboBox;
    m_comboBox->setView(comView);

    m_treeModel = new TreeVideoModel(this);
    m_treeView = new TreeVideoView;
    m_treeView->setHeaderHidden(true);
    m_treeView->header()->setStretchLastSection(true);
    m_treeView->setDragEnabled(true);
    m_treeView->setEditTriggers(QTreeView::AllEditTriggers);
    m_treeView->setModel(m_treeModel);

    m_comboBox->setFixedSize(108, 30);
    m_comboBox->setProperty("outer_stylesheet", "search");
    m_lineEdit->setFixedHeight(30);
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_comboBox);
    hlayout->addWidget(m_lineEdit, 1);
    hlayout->setSpacing(0);
    hlayout->setMargin(0);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_treeView, 1);
    vlayout->setSpacing(0);
    vlayout->setMargin(0);

    setLayout(vlayout);

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSelectStation(int)));
    connect(m_lineEdit->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(slotSearchCamera(QString)));
    connect(m_treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(slotUpdateAndExpandNode(QModelIndex)));
    connect(m_treeView, SIGNAL(signalRefresh()), this, SLOT(slotInitAll()));
    connect(m_treeView, SIGNAL(signalSettings()), this, SLOT(slotSettings()));
    connect(m_treeView, SIGNAL(signalExportJson()), this, SLOT(slotExportJson()));
    connect(m_treeView, SIGNAL(signalImportJson()), this, SLOT(slotImportJson()));
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

void TreeVideoViewSearch::updateItemTipText(QStandardItem *item)
{
    item->setToolTip(
                QString::fromUtf8("名称：%1\n编号：%2\n类型：%3\n状态：%4\n地址：%5")
                .arg(item->data(VideoNameRole).toString())
                .arg(item->data(VideoObidRole).toString())
                .arg(item->data(VideoTypeNameRole).toString())
                .arg(item->data(VideoStateNameRole).toString())
                .arg(item->data(VideoUrlRole).toString())
                );
}

void TreeVideoViewSearch::slotInitAll()
{
    initTree();
}

void TreeVideoViewSearch::slotInitTree()
{
    m_comboBox->clear();
    m_comboBox->addItem(QString::fromUtf8("所有"), "%");

    m_treeModel->clear();
    QStandardItem *itemRoot =  m_treeModel->invisibleRootItem();
    QStandardItem *itemISCS = createItem(QString::fromUtf8("摄像头"), Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    itemRoot->setChild(0, 0, itemISCS);
    appendHeaderHorizontalItem(itemRoot);

    for(const DataSource::Location &d : m_datasource->getLocationList()) {
        m_comboBox->addItem(d.name, d.obid);

        QStandardItem *item_location = new QStandardItem;
        item_location->setText(d.name);
        item_location->setData(VideoNodeStation, VideoNodeType);
        item_location->setData(d.obid,    VideoObidRole);
        item_location->setData(d.name,    VideoNameRole);
        item_location->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemISCS->appendRow(item_location);

        //占位用
        if(d.camera_count > 0)
            item_location->setChild(0, 0, new QStandardItem);
    }
    //
    m_treeView->expand(m_treeModel->index(0, 0));
}

void TreeVideoViewSearch::slotSettings()
{
    QDialog d(this);
    d.setObjectName("Window");
    QGridLayout *vlayout = new QGridLayout;
    QFile file(lds::configDirectory + "/skin.qss");
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
    QFile file(lds::configDirectory + "/skin.qss");
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();

    QSet<QString> colorSet;
    for(QObject *o : w->children())
    {
        QString text = o->property("text").toString();
        if(text.startsWith("#")) {
            colorSet.insert(text.mid(0, 7));
        }
    }
    //
    for(QObject *o : w->children())
    {
        QString text = o->property("text").toString();
        if(text.startsWith("#") && text.contains("->")) {
            QString l = text.split("->").value(0);
            QString r = text.split("->").value(1);
            while(colorSet.contains(r)) {
                int value = QColor(r).value();
                if(value == 255)
                    value --;
                else
                    value ++;
                r = QColor::fromHsv(QColor(r).hue(), QColor(r).saturation(), value).name();
            }
            colorSet.insert(r);
            all.replace(l, r.toLocal8Bit());
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
}

void TreeVideoViewSearch::slotImportJson()
{
}

void TreeVideoViewSearch::addToPlayThread(const QString &url)
{

}

void TreeVideoViewSearch::initTree()
{
    slotInitTree();
}

void TreeVideoViewSearch::appendHeaderHorizontalItem(QStandardItem *itemRoot)
{
}

void TreeVideoViewSearch::appendDeviceHorizontalItem(QStandardItem *item_location, int row, QStandardItem *item_device)
{
}

void TreeVideoViewSearch::slotSelectStation(int index)
{
    QString location_obid = m_comboBox->itemData(index).toString();
    if("%" == location_obid) {
        m_treeView->scrollToTop();
        return;
    }

    updateCameraItemListOnce(location_obid);

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
    QStandardItem *itemRoot     = m_treeModel->invisibleRootItem();
    QStandardItem *itemISCS     = itemRoot->child(0);
    for(int location_row = 0; location_row < itemISCS->rowCount(); location_row ++) {
        QStandardItem *item_location = itemISCS->child(location_row);
        for(int camera_row = 0; camera_row < item_location->rowCount(); camera_row ++) {
            QStandardItem *item_camera = item_location->child(camera_row);
            //未展开的节点
            if(item_camera->data(VideoObidRole).toString() == "") {
                camera_row --;
                updateCameraItemList(item_location);
                continue;
            }

            if(item_camera->data(VideoNameRole).toString().contains(text)) {
                m_treeView->setCurrentIndex(item_camera->index());
                return;
            }
        }
    }
}

void TreeVideoViewSearch::updateCameraItemListOnce(const QString &location_obid)
{
    QStandardItem *item = getLocationItem(location_obid);
    //若第一个设备的obid为空，则需要加载数据
    if(item && item->child(0) && item->child(0)->data(VideoObidRole).toString().isEmpty()) {
        updateCameraItemList(location_obid);
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
    for(const DataSource::Camera &d : m_datasource->getCameraList(location_obid)) {
        QString url = m_datasource->getCameraUrl(d.obid);//d.url;

        QString stateName = m_datasource->getCameraStateName(d.state);
        QString typeName = m_datasource->getCameraTypeName(d.type);

        int row = item_location->rowCount();
        QStandardItem *item_device = new QStandardItem;
        item_device->setText(d.name);
        item_device->setData(VideoNodeDevice,       VideoNodeType);
        item_device->setData(d.name,                VideoNameRole);
        item_device->setData(typeName,              VideoTypeNameRole);
        item_device->setData(d.type,                VideoTypeRole);
        item_device->setData(d.obid,                VideoObidRole);
        item_device->setData(d.state,               VideoStateRole);
        item_device->setData(stateName,             VideoStateNameRole);
        item_device->setData(url,               VideoUrlRole);
        item_device->setData(getCameraStatePixmap(d.state),   Qt::DecorationRole);
        item_device->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

        updateItemTipText(item_device);

        item_location->setChild(row, 0, item_device);
        //
        appendDeviceHorizontalItem(item_location, row, item_device);
        addToPlayThread(url);
    }
}

void TreeVideoViewSearch::slotUpdateAndExpandNode(const QModelIndex &index)
{
    if(index.data(VideoNodeType).toInt() == VideoNodeStation) {
        QString obid = index.data(VideoObidRole).toString();
        updateCameraItemListOnce(obid);
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
