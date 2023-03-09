#ifndef MAINSYSTEMSETINGSURL_H
#define MAINSYSTEMSETINGSURL_H

#include <QWidget>

namespace Ui {
class MainSystemSetingsUrl;
}
class DataSource;

class MainSystemSetingsUrl : public QWidget
{
    Q_OBJECT

public:
    explicit MainSystemSetingsUrl(QWidget *parent = 0);
    ~MainSystemSetingsUrl();

    void refresh();

    void setDataSource(DataSource *datasource);
private:
    Ui::MainSystemSetingsUrl *ui;
    DataSource *m_datasource;
};

#endif // MAINSYSTEMSETINGSURL_H
