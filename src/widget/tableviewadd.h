#ifndef TABLEVIEWADD_H
#define TABLEVIEWADD_H

#include <QTableView>
#include <QStandardItemModel>

class TableViewAdd : public QTableView
{
    Q_OBJECT
public:
    TableViewAdd(QWidget *parent = 0);

    QStringList result();
    void setStringList(const QStringList &list);
signals:
    void editingFinished();

public slots:
    void toCheckTable(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

    void toAdd();
    void toRemove();
private:
    QStandardItemModel *m_tableModel;
};

#endif // TABLEVIEWADD_H
