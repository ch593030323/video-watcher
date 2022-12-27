#ifndef CHAPPLICATION_H
#define CHAPPLICATION_H
#include <oms/data_all.h>
#include <oms/oms_database.h>
#include <oms/qt_core_application.h>
#include "oms/qt_application.h"
//#include <QWidget>

class ChApplication : public QtApplication
{
    Q_OBJECT
public:
    // explicit ChApplication(QWidget *parent = 0);
    ChApplication(int argc, char** argv, SignalHandler sigtermHandler,
        const OptionList& optionList, EnvironmentScope environmentScope);
    ~ChApplication();
    static const int MAIN_INTERVAL;
public:
    OMSDatabase* getOMS() const;
    bool initApp();
    void exitApp();
    void waitToExit();
    bool isRunning() const;
    bool getAllTypes();
signals:

public slots:

private:
    OMSDatabase* m_pOMS;
    bool m_bAppIsRunning;

};

#endif // CHAPPLICATION_H
