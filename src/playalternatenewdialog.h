#ifndef PLAYALTERNATENEWDIALOG_H
#define PLAYALTERNATENEWDIALOG_H

#include <QDialog>

namespace Ui {
class PlayAlternateNewDialog;
}

class QStandardItemModel;
class PlayAlternateNewDialog : public QDialog
{
    Q_OBJECT

    enum AlterType{
        CameraType = 0,
        TimeGapType
    };

    struct AlterData{
         AlterType type;
         QString value;
    };
public:
    explicit PlayAlternateNewDialog(QWidget *parent = 0);
    ~PlayAlternateNewDialog();

    void setTreeModel(QStandardItemModel *model);

private slots:
    void toadd();
    void todel();
    void toaddgap();

    void took();
    void toexit();
    void toadjust();
private:
    Ui::PlayAlternateNewDialog *ui;
};

#endif // PLAYALTERNATENEWDIALOG_H
