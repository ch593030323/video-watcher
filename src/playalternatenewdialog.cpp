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
    ui->setupUi(this);
    setObjectName("Window");
    ui->spinBox->setMinimum(1);
    resize(832, 557);
    //
    ui->pushButton_ok->setIcon(lds::getFontPixmap(0x2713));
    ui->pushButton_exit->setIcon(lds::getFontPixmap(0xd7));

    ui->pushButton_ok->setText(type == TypeNew ? QString::fromUtf8("新增") : QString::fromUtf8("修改"));
    ui->lineEdit->setText(lds::getUniqueFileNamehByDateTime("play_alter"));

    ui->pushButton_add->setIcon(lds::getFontPixmap(0x203a, PropertyColor::buttonTextColor));
    ui->pushButton_del->setIcon(lds::getFontPixmap(0x203a, PropertyColor::buttonTextColor));
    ui->pushButton_add_gap->setIcon(lds::getFontPixmap(0x203a, PropertyColor::buttonTextColor));

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(toadd()));
    connect(ui->pushButton_add_gap, SIGNAL(clicked()), this, SLOT(toaddgap()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_adjust, SIGNAL(clicked()), this, SLOT(toadjust()));

    ui->pushButton_adjust->hide();
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
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
    }
    ui->listWidget->clear();
    //更新listwidget的内容
    while(!file.atEnd()) {
        QString line = file.readLine().trimmed();
        int index = line.indexOf(":");
        QString key = line.mid(0, index);
        QString value = line.mid(index + 1);
        if(key.isEmpty() || value.isEmpty())
            continue;
        insertAlterData((AlterType)key.toInt(), value);
    }

    ui->lineEdit->setText(QFileInfo(filepath).baseName());
}

void PlayAlternateNewDialog::insertAlterData(AlterType type, const QString &value)
{
    int row_next = ui->listWidget->currentRow() < 0 ? ui->listWidget->count() : (ui->listWidget->currentRow() + 1);

    QListWidgetItem *item = new QListWidgetItem;
    item->setText((type == CameraUrlType ? "url:" : "timeout:") + value);
    item->setData(AlterTypeRole, type);
    item->setData(AlterValueRole, value);

    ui->listWidget->insertItem(row_next, item);
}

void PlayAlternateNewDialog::toadd()
{
    QModelIndex index = ui->treeView->currentIndex();
    if(!index.isValid())
        return;
    if(index.data(VideoNodeType).toInt() != VideoNodeDevice)
        return;

    insertAlterData(CameraUrlType,index.data(VideoUrlRole).toString());
}

void PlayAlternateNewDialog::todel()
{
    int row = ui->listWidget->currentRow();
    if(row < 0)
        return;
    delete ui->listWidget->takeItem(row);
}

void PlayAlternateNewDialog::toaddgap()
{
    insertAlterData(TimeGapType, QString::number(ui->spinBox->value()));
}

void PlayAlternateNewDialog::took()
{
//    toadjust();

    QString content;
    for(int row = 0; row < ui->listWidget->count(); row ++) {
        QListWidgetItem *item = ui->listWidget->item(row);
        content += QString("%1:%2\n")
                .arg(item->data(AlterTypeRole).toInt())
                .arg(item->data(AlterValueRole).toString());
    }

    QDir().mkpath("play_alter");
    QFile file(QString("play_alter/%1.txt").arg(ui->lineEdit->text().trimmed()));
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << file.errorString();
    }
    file.write(content.toLocal8Bit());
    file.close();

    this->accept();
}

void PlayAlternateNewDialog::toexit()
{
    this->reject();
}

void PlayAlternateNewDialog::toadjust()
{
    //合并
    QList<AlterData> lineList;
    for(int row = 0; row < ui->listWidget->count(); row ++) {
        QListWidgetItem *item = ui->listWidget->item(row);
        AlterData line2;
        line2.type = (AlterType)item->data(AlterTypeRole).toInt();
        line2.value = item->data(AlterValueRole).toString();

        if(row == 0) {
            lineList << line2;
            continue;
        }
        AlterData &line = lineList.last();
        if(line2.type == TimeGapType && line.type == TimeGapType) {
            line.value = QString::number(line.value.toInt() + line2.value.toInt());
            continue;
        }
        if(line2.type == CameraUrlType && line.type == CameraUrlType) {
            line.value = line2.value;
            continue;
        }
        lineList << line2;
    }
    //移除
    if(lineList.count() >= 2) {
        if(lineList[0].type == TimeGapType) {
            lineList.append(lineList.takeFirst());
        }
        if(lineList[0].type == CameraUrlType && lineList.last().type == CameraUrlType) {
            lineList.removeLast();
        }
    }
    //更新
    ui->listWidget->clear();
    for(int k = 0; k < lineList.count(); k ++) {
        const AlterData &line = lineList[k];
        QListWidgetItem *item = new QListWidgetItem;

        if(line.type == TimeGapType) {
            item->setText(QString::fromUtf8("间隔:") + line.value + "s");
            item->setData(AlterTypeRole, line.type);
            item->setData(AlterValueRole, line.value);
        }
        if(line.type == CameraUrlType) {
            item->setText("url:" + line.value);
            item->setData(AlterTypeRole, line.type);
            item->setData(AlterValueRole, line.value);
        }

        ui->listWidget->addItem(item);
    }
}
