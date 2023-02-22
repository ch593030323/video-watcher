#ifndef LDS_TABLEVIEW_H
#define LDS_TABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>

class lds_tableview : public QTableView
{
    Q_OBJECT
public:
    explicit lds_tableview(QWidget *parent = nullptr);

    void appHorizontalHeader(const QString &key, const QString &text, int width);
    int fieldIndex(const QString &key);

    QVariant data(int row, const QString &key);

    bool removeRows(int begin, int count);
    void appendRow(const QStringList &texts);

    int rowCount();
private:
    QStandardItemModel *model;
    QMap<QString, int> horizontalHeaderMap;
};

#endif // LDS_TABLEVIEW_H
