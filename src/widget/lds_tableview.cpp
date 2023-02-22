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

void lds_tableview::appendRow(const QStringList &texts)
{
    QList<QStandardItem *> items;
    for(const QString &text : texts) {
        items << new QStandardItem(text);
    }
    model->appendRow(items);
}

int lds_tableview::rowCount()
{
    return model->rowCount();
}
