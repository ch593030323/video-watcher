#include "chapplication.h"
const int ChApplication::MAIN_INTERVAL = 2;
/*
ChApplication::ChApplication(QWidget *parent) : QWidget(parent)
{

}
*/
ChApplication::ChApplication(int argc,
    char** argv,
    SignalHandler sigtermHandler,
    const OptionList& optionList,
    EnvironmentScope environmentScope)
    : QtApplication(argc, argv, sigtermHandler, optionList, environmentScope)
    , m_pOMS(NULL)
{

}

ChApplication::~ChApplication()
{

}

//m_pOMS = static_cast<OMSDatabase*>(getDatabase());

OMSDatabase *ChApplication::getOMS() const
{

}
bool ChApplication::initApp()
{
    m_pOMS = static_cast<OMSDatabase*>(getDatabase());
    if (!m_pOMS || !getIpcChannel())
        return false;
    //if (!getAllTypes())
    //    return false;
    m_bAppIsRunning = true;
    return true;
}

void ChApplication::exitApp()
{
    m_bAppIsRunning = false;
    //m_sLog = "waitting to exit app";
    waitToExit();
    endSvc();
}

void ChApplication::waitToExit()
{
    m_bAppIsRunning = false;
    //m_sLog = "waitting to exit app";
    //outputDebugLog(m_sLog);
    ::sleep(ChApplication::MAIN_INTERVAL);
}
bool ChApplication::isRunning() const
{
    return m_bAppIsRunning;
}
