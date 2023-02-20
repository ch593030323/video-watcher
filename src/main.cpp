#include "mainwindow.h"
#include "lds.h"
#include "json/json.h"
//#include "QJsonDocument.h"
//#include "QJsonObject.h"
//#include "QJsonArray.h"
#include "widget.h"

#include <QtDebug>
#include <QApplication>
#include <QTreeView>
#include <QStandardItemModel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif

//    lds::init();

    MainFrame w;
    w.resize(1000, 700);
    w.show();
    return a.exec();
}
