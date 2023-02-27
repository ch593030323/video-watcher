#ifndef PLAYALTERNATENEWDIALOG_H
#define PLAYALTERNATENEWDIALOG_H

#include "lds.h"
#include "datasource.h"

#include <QDialog>
#include <QStandardItemModel>


namespace Ui {
class PlayAlternateNewDialog;
}


class PlayAlternateNewDialog : public QDialog
{
    Q_OBJECT

    enum AlterType{
        CameraUrlType = 0,
        TimeGapType
    };

    struct AlterData{
         AlterType type;
         QString value;
    };
public:
    enum Type{
        TypeNew,
        TypeModify
    };
public:
    explicit PlayAlternateNewDialog(Type type, QWidget *parent = 0);
    ~PlayAlternateNewDialog();

    void setDataSource(DataSource *datasource);

    void readFrom(const QString &filepath);

private:
    void insertAlterData(AlterType type, const QString &value);

private slots:
    void toadd();
    void todel();

    void took();
    void toexit();

private:
    Ui::PlayAlternateNewDialog *ui;
    Type m_type;
};

#endif // PLAYALTERNATENEWDIALOG_H
