#include "lds_tableview.h"

#include <QtDebug>

lds_tableview::lds_tableview(QWidget *parent) : QTableView(parent)
{
    model = new QStandardItemModel(this);
    setModel(model);
}

void lds_tableview::appHorizontalHeader(const QString &key, const QString &text, int width)
{
    int column = model->columnCount();
    model->setColumnCount(column + 1);

    model->setHeaderData(column, Qt::Horizontal, text);
    horizontalHeaderMap[key] = column;

    setColumnWidth(column, width);

}

int lds_tableview::fieldIndex(const QString &key)
{
    return horizontalHeaderMap.value(key, -1);
}

QVariant lds_tableview::data(int row, const QString &key)
{
    return model->data(model->index(row, fieldIndex(key)));
}

bool lds_tableview::removeRows(int begin, int count)
{
    return model->removeRows(begin, count);
}

bool lds_tableview::removeRow(int begin)
{
    return model->removeRows(begin, 1);
}

void lds_tableview::appendRow(const QStringList &texts)
{
    insertRow(model->rowCount(), texts);
}

void lds_tableview::insertRow(int row, const QStringList &texts)
{
    QList<QStandardItem *> items;
    for(const QString &text : texts) {
        items << new QStandardItem(text);
    }
    model->insertRow(row, items);
}

void lds_tableview::insertRow(int row, const QString &text)
{
    insertRow(row, QStringList() << text);
}

void lds_tableview::insertRow(int row, QStandardItem *item)
{
    model->insertRow(row, item);
}

QStandardItem *lds_tableview::takeItem(int row, int column)
{
    return model->takeItem(row, column);
}

QStandardItem *lds_tableview::item(int row, int column)
{
    return model->item(row, column);
}

int lds_tableview::rowCount()
{
    return model->rowCount();
}
