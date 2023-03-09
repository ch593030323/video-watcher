#include "editedwidgetmapper.h"

#include <QtDebug>

EditedWidgetMapper::EditedWidgetMapper(QObject *parent)
    : QObject(parent)
    , m_hasEditedWidget(false)
{
}

void EditedWidgetMapper::addObject(QObject *target, const char *signal)
{
    connect(target, signal, this, SLOT(slotEdited()));
}

bool EditedWidgetMapper::hasEditedWidget()
{
    return m_hasEditedWidget;
}

void EditedWidgetMapper::reset()
{
    m_hasEditedWidget = false;
}

void EditedWidgetMapper::slotEdited()
{
    m_hasEditedWidget = true;
}
