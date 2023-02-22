#ifndef PLAYFORMNEWDIALOG_H
#define PLAYFORMNEWDIALOG_H

#include "lds.h"
#include "videolayout.h"
#include "datasource.h"

#include <QDialog>
#include <QMap>

class VideoCell;

namespace Ui {
class playformnewdialog;
}

class playformnewdialog : public QDialog
{
    Q_OBJECT
public:
    enum Type{
        TypeNew,
        TypeModify
    };

public:
    explicit playformnewdialog(Type type, QWidget *parent = 0);
    ~playformnewdialog();

    void setDataSource(DataSource *datasource);

    void updateLayout();

    void readFrom(const QString &filepath);

protected:
    void contextMenuEvent(QContextMenuEvent * event);

    void resizeEvent(QResizeEvent *event);

private slots:
    void toVideoLayout1x1();
    void toVideoLayout2x2();
    void toVideoLayout3x3();
    void toVideoLayout4x4();
    void toVideoLayout5x5();

    void toaccept();
    void toexit();

private:
    Ui::playformnewdialog *ui;
    Type m_type;
};

#endif // PLAYFORMNEWDIALOG_H
