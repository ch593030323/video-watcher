#ifndef EDITEDWIDGETMAPPER_H
#define EDITEDWIDGETMAPPER_H

#include <QObject>

class EditedWidgetMapper : public QObject
{
    Q_OBJECT
public:
    explicit EditedWidgetMapper(QObject *parent = 0);

    void addObject(QObject *target, const char *signal);

    bool hasEditedWidget();

    void reset();

private slots:
    void slotEdited();

private:
    bool m_hasEditedWidget;
};

#endif // EDITEDWIDGETMAPPER_H
