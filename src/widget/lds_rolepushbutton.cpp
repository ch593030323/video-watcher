#include "lds_rolepushbutton.h"

lds_rolePushButton::lds_rolePushButton(QWidget *parent) : QPushButton(parent)
{

}

void lds_rolePushButton::setRoleData(int role, const QVariant &data)
{
    m_map.insert(role, data);
}

QVariant lds_rolePushButton::roleData(int role)
{
    return m_map.value(role);
}
