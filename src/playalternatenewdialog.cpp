#include "playalternatenewdialog.h"
#include "ui_playalternatenewdialog.h"
#include "lds.h"
#include "treevideomodel.h"
#include "propertycolor.h"

#include <QStandardItemModel>
#include <QDir>
#include <QFile>
#include <QtDebug>

PlayAlternateNewDialog::PlayAlternateNewDialog(PlayAlternateNewDialog::Type type, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlayAlternateNewDialog)
    , m_type(type)
{
    ui->setupUi(this);this->setWindowTitle(lds::windowName);
    setObjectName("Window");
    ui->spinBox->setMinimum(1);

    ui->tableView->appHorizontalHeader("url", QString::fromUtf8("流地址(url)"), 250);
    ui->tableView->appHorizontalHeader("length", QString::fromUtf8("播放时间(s)"), 50);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    resize(832, 557);
    //
    ui->pushButton_ok->setIcon(lds::getFontPixmap(0x2713));
    ui->pushButton_exit->setIcon(lds::getFontPixmap(0xd7));

    ui->pushButton_ok->setText(type == TypeNew ? QString::fromUtf8("新增") : QString::fromUtf8("修改"));
    if(type == TypeModify) {
        ui->lineEdit->setEnabled(false);
    }
    ui->lineEdit->setText(lds::getUniqueFileNamehByDateTime(lds::configDirectory + "/play_alter"));

    ui->pushButton_add->setIcon(lds::getFontPixmap(0x203a, PropertyColor::buttonTextColor));
    ui->pushButton_del->setIcon(lds::getFontPixmap(0x203a, PropertyColor::buttonTextColor));

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(toadd()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
}

PlayAlternateNewDialog::~PlayAlternateNewDialog()
{
    delete ui;
}

void PlayAlternateNewDialog::setDataSource(DataSource *datasource)
{
    ui->treeView->setDataSource(datasource);
    ui->treeView->slotInitTree();
}

void PlayAlternateNewDialog::readFrom(const QString &filepath)
{
    ui->tableView->removeRows(0, ui->tableView->rowCount());

    QList<AlterPlayFrame> list = AlterPlayFrame::readFrom(filepath);
    for(int k = 0; k < list.count(); k ++) {
        ui->tableView->appendRow({list[k].url, QString::number(list[k].length)});
    }

    ui->lineEdit->setText(QFileInfo(filepath).baseName());
}

void PlayAlternateNewDialog::insertAlterData(AlterType type, const QString &value)
{
    int row_cur = ui->tableView->currentIndex().row();
    int row_count = ui->tableView->rowCount();
    int row_next = row_cur < 0 ? row_count : (row_cur + 1);

    QStandardItem *item = new QStandardItem;
    item->setText((type == CameraUrlType ? "url:" : "timeout:") + value);
    item->setData(type, AlterTypeRole);
    item->setData(value, AlterValueRole);

    ui->tableView->insertRow(row_next, item);
}

void PlayAlternateNewDialog::toadd()
{
    QModelIndex index = ui->treeView->currentIndex();
    if(!index.isValid())
        return;
    if(index.data(VideoNodeTypeRole).toInt() != NodeTypeDevice)
        return;
    ui->tableView->appendRow({index.data(VideoUrlRole).toString(),
                             QString::number(ui->spinBox->value())});
}

void PlayAlternateNewDialog::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0)
        return;
    ui->tableView->removeRow(row);
}

void PlayAlternateNewDialog::took()
{
    QString content;
    for(int row = 0; row < ui->tableView->rowCount(); row ++) {
        content += QString("%1:%2\n")
                .arg(ui->tableView->data(row, "length").toString())
                .arg(ui->tableView->data(row, "url").toString());
    }

    QDir().mkpath(lds::configDirectory + "/play_alter");
    QFile file(lds::configDirectory + QString("/play_alter/%1.txt").arg(ui->lineEdit->text().trimmed()));
    if(!file.open(QFile::WriteOnly)) {
        lds::showMessage((file.errorString() + ":" + file.fileName()));
    }
    file.write(content.toLocal8Bit());
    file.close();

    this->accept();
}

void PlayAlternateNewDialog::toexit()
{
    this->reject();
}
