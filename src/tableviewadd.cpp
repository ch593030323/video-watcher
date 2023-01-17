#include "tableviewadd.h"

#include <QHeaderView>

TableViewAdd::TableViewAdd(QWidget *parent)
    : QTableView(parent)
{
    m_tableModel = new QStandardItemModel(this);
    m_tableModel->setHorizontalHeaderLabels({""});
    this->setModel(m_tableModel);
    this->horizontalHeader()->hide();
    this->horizontalHeader()->setStretchLastSection(true);
    this->setEditTriggers(QTableView::SelectedClicked);

    connect(this->itemDelegate(), SIGNAL(closeEditor(QWidget *, QAbstractItemDelegate::EndEditHint)),
            this, SLOT(toCheckTable(QWidget *, QAbstractItemDelegate::EndEditHint)));


}

QStringList TableViewAdd::result()
{
    QStringList list;
    for(int k = 0; k < m_tableModel->rowCount(); k ++) {
        list << m_tableModel->index(k, 0).data().toString();
    }
    return list;
}

void TableViewAdd::toCheckTable(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
{
    if(editor && editor->property("text").toString().trimmed().isEmpty()) {
        m_tableModel->removeRow(m_tableModel->rowCount() - 1);
    }
}

void TableViewAdd::toAdd()
{
    int row = m_tableModel->rowCount();
    if(row > 0 && m_tableModel->index(row - 1, 0).data().toString().trimmed().isEmpty()) {
        m_tableModel->removeRow(row -1);
    }

    m_tableModel->insertRow(row);
    this->selectRow(row);
    this->setCurrentIndex(m_tableModel->index(row, 0));
    this->edit(m_tableModel->index(row, 0));
}

void TableViewAdd::toRemove()
{
    m_tableModel->removeRow(this->currentIndex().row());

}
