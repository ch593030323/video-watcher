#include "mainwidget.h"
#include "src/lds.h"

#include <QApplication>
#include <QWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    lds::init();

    mainWidget w;
    w.setStyleSheet(lds::styleSheetString);
    w.show();
    return a.exec();
}
