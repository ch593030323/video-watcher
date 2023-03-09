#ifndef MAINSYSTEMSETINGSSNAPLISTVIEW_H
#define MAINSYSTEMSETINGSSNAPLISTVIEW_H

#include <QListView>

class MainSystemSetingsSnapListView : public QListView
{
    Q_OBJECT
public:
    MainSystemSetingsSnapListView(QWidget *parent = 0);

signals:
    void signalDelete();
    void signalRefresh();

protected:
    void contextMenuEvent(QContextMenuEvent * event);

};

#endif // MAINSYSTEMSETINGSSNAPLISTVIEW_H
