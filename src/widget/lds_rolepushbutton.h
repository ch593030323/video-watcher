#ifndef LDS_ROLEPUSHBUTTON_H
#define LDS_ROLEPUSHBUTTON_H

#include <QPushButton>
#include <QMap>
#include <QVariant>

class lds_rolePushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit lds_rolePushButton(QWidget *parent = 0);

    void setRoleData(int role ,const QVariant &data);
    QVariant roleData(int role);
private:
    QMap<int, QVariant> m_map;
};

#endif // LDS_ROLEPUSHBUTTON_H
