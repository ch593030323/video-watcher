#ifndef LDS_TABLEVIEW_H
#define LDS_TABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>

class lds_tableview : public QTableView
{
    Q_OBJECT
public:
    explicit lds_tableview(QWidget *parent = 0);

    void appHorizontalHeader(const QString &key, const QString &text, int width);
    int fieldIndex(const QString &key);

    QVariant data(int row, const QString &key, int role = Qt::DisplayRole);
    bool setData(int row, const QString &key, const QVariant &var, int role = Qt::EditRole);

    bool removeRows(int begin, int count);
    bool removeRow(int begin);

    void appendRow(const QStringList &texts);

    void insertRow(int row, const QStringList &texts);
    void insertRow(int row, const QString &text);
    void insertRow(int row, QStandardItem *item);

    QStandardItem *takeItem(int row, int column = 0);

    QStandardItem *item(int row, int column = 0);

    int rowCount();
private:
    QStandardItemModel *model;
    QMap<QString, int> horizontalHeaderMap;
};

#endif // LDS_TABLEVIEW_H
