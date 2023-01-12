#include "mainwidget.h"
#include "src/lds.h"

#include <QApplication>
#include <QWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
        QFont font = qApp->font();
        font.setPixelSize(25);
        qApp->setFont(font);

    lds::init();
    lds::getFontPixmap(0xf057, QColor("#f6f5fb"), QSize(20, 20)).save("circle_xmark_20x20.png");
    lds::getFontPixmap(0xf057, QColor("#ff506b"), QSize(20, 20)).save("circle_xmark_pressed_20x20.png");

    mainWidget w;
    w.setStyleSheet(lds::styleSheetString);
    w.show();
    return a.exec();
}
