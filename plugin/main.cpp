#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include "chapplication.h"
#include <QtCore>
#include <QtGui>
#include <QWidget>

#include "VideoWatcher_plugin.h"
#include "VideoWatcher_widget.h"
#define MAIN_INTERVAL 2
ChApplication* g_pApp = NULL;
extern "C" {
    void SigtermHandler(int nCode)
    {

        if (nCode == SIGTERM) {
            qDebug() << "\n#Exiting due to SIGTERM.";
        }
        if (nCode == SIGINT) {
            qDebug() << "\n#Exiting due to SIGINT.";
        }
        if (g_pApp) {
            g_pApp->exitApp();
            ::sleep(MAIN_INTERVAL);
        } else {
            return;
        }
    }
}

void showVersion()
{
    QString sVersion("V1.0.0.20211112");
    QString sChangeLog("1.First release version after interface testing.");
    printf("###Version###\n");
    printf("%s\n", sVersion.toAscii().data());
    printf("###ChangeLog###\n");
    printf("%s\n", sChangeLog.toAscii().data());
}

void setCodec()
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (QString(argv[i]) == "-v" || QString(argv[i]) == "-version") {
            showVersion();
            return 0;
        }
    }
    setCodec();

    OptionList optionlist;
    g_pApp = new ChApplication(argc, argv, SigtermHandler, optionlist, RUNTIME_SCOPE);
    if (!g_pApp->initApp()) {
        qDebug() << "Error: initialize app";
        //g_ErrorLog->writeLog("Error: initialize app");
        g_pApp->exitApp();
        return 0;
    }

    VideoWatcherPlugin p;
    QWidget *w = p.createWidget(NULL, g_pApp->getDatabase());
    w->resize(1000, 700);
    w->show();

    if (g_pApp->isRunning()) {
        qDebug() << "App svc...";
        g_pApp->svc();
    }
}
