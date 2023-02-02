#include "omsquery.h"
#include "chapplication.h"

#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include <QtCore>
#include <QtGui>
#include <QWidget>

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

QString idName(QString id, QString name) {
    return id.isEmpty() ? name : (name + "(" + id + ")");
}

void showHelp()
{
    printf("Description: \n"
           "\tlist a bill of object's two different attribute. \n"
           "Usage: \n"
           "\tGD_DataCompare [oname|oid] [aname|aid] [aname|aid]\n"
           "Options:\n"
           //"\t -d, --detail \tshow a detail bill\n"
           "\t -h, --help \tshow help information\n"
           );
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; ++i) {
        if (QString(argv[i]) == "-v" || QString(argv[i]) == "-version") {
            showVersion();
            return 0;
        }
        if (QString(argv[i]) == "-h" || QString(argv[i]) == "--help") {
            showHelp();
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
	
	//清屏
    system("reset \n");
    OMSDatabase *database = static_cast<OMSDatabase*>(g_pApp->getDatabase());
    if(!database) {
        qDebug() << "Error: OMSDatabase is NULL";
        g_pApp->exitApp();
        return 0;
    }
    OMSQuery query(database);
    OMSQuery query_choice(database);
    QString table, table_id;//oject
    QString attr1, attr1_id;//attribute
    QString attr2, attr2_id;//attribute
    for(int k = 1; k < argc; k ++) {
        if(k == 1) table = argv[k];
        if(k == 2) attr1 = argv[k];
        if(k == 3) attr2 = argv[k];
    }

    //获取oname或oid、aname或aid
    try {
        if(table.toLongLong() > 0) {
            table_id = table;
            table =  database->matchOName(table.toUInt());
        }

        if(attr1.toLongLong() > 0) {
            attr1_id = table;
            attr1 =  database->matchAName(attr1.toUInt());
        }
        if(attr2.toLongLong() > 0) {
            attr2_id = table;
            attr2 =  database->matchAName(attr2.toUInt());
        }
    } catch(Exception &e) {
        qDebug() << QString::fromStdString(e.getDescription());
        g_pApp->exitApp();
        return 0;
    }

    //select result
    bool ok = query.exec("select obid, " + attr1 + "," + attr2 + " from " + table);
    if(!ok) {
        qDebug() << query.lastError();
        g_pApp->exitApp();
        return 0;
    }

    //DataCompare CCTVController Status CommState
    int count = 0;
    QString table_obid;
    while(query.next()) {
        table_obid         = query.value("obid").toString();
        int dataType       = query.value(attr1, RoleDataType).toInt();
        QString dataValue  = query.value(attr1).toString();

        int dataType2      = query.value(attr2, RoleDataType).toInt();
        QString dataValue2 = query.value(attr2).toString();

        if(dataValue != dataValue2) {
            if(dataType == MULTIPLE_CHOICE_DATA) {
                dataValue = query_choice.getChoiceData(table, attr1).value(dataValue.toInt());
            }
            if(dataType2 == MULTIPLE_CHOICE_DATA) {
                dataValue2 = query_choice.getChoiceData(table, attr2).value(dataValue2.toInt());
            }
            qDebug() << QString("For object %1 %2=%3 %4=%5 not match")
                        .arg(idName(table_obid, table))
                        .arg(attr1)
                        .arg(dataValue)
                        .arg(attr2)
                        .arg(dataValue2);
            count ++;
        }

    }
    qDebug() << QString("Total not matched objects count is %1")
                .arg(count);

    g_pApp->exitApp();
    return 0;
}


/*
命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare CCTVController Status CommState
输出：
"For object CCTVController(2022929596417) Status=离线 CommState=通信故障 not match"
"Total not matched objects count is 1"
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup


命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare 471 Status CommState
输出：
"For object CCTVController(2022929596417) Status=离线 CommState=通信故障 not match"
"Total not matched objects count is 1"
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup


命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare 471 276 CommState
输出：
"For object CCTVController(2022929596417) Status=离线 CommState=通信故障 not match"
"Total not matched objects count is 1"
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup

命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare 471 Status 2343
输出：
"For object CCTVController(2022929596417) Status=离线 CommState=通信故障 not match"
"Total not matched objects count is 1"
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup

命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare CCTVController Status1 CommState
输出：
"ANameException: Status1"
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup


命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare 787683674 Status CommState
输出：
"OTypeException: 787683674"
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup


命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare 442 547 548
输出：
"OMSQueryParser Error: data type 'TIME_DATA' cannot be resolved."
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup


命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare -h
输出：
Description:
    list a bill of object's two different attribute.
Usage:
    GD_DataCompare [oname|oid] [aname|aid] [aname|aid]
Options:
     -h, --help 	show help information

命令：/home/cps/cz/build-DataCompare-unknown-Debug/GD_DataCompare CCTVController Address PointAddress
输出：
"For object CCTVController(2022929596417) Address=100 PointAddress=10 not match"
"For object CCTVController(2022929596419) Address=2 PointAddress=10 not match"
"For object CCTVController(2022929596420) Address=3 PointAddress=10 not match"
"For object CCTVController(2022929596421) Address=4 PointAddress=10 not match"
"For object CCTVController(2022929596422) Address=5 PointAddress=10 not match"
"For object CCTVController(2022929596423) Address=6 PointAddress=10 not match"
"For object CCTVController(2022929596424) Address=7 PointAddress=10 not match"
"For object CCTVController(2022929596425) Address=8 PointAddress=10 not match"
"For object CCTVController(2022929596426) Address=9 PointAddress=10 not match"
"For object CCTVController(2022929596428) Address=11 PointAddress=10 not match"
"For object CCTVController(2022929596429) Address=12 PointAddress=10 not match"
"For object CCTVController(2022929596430) Address=13 PointAddress=10 not match"
"For object CCTVController(2022929596431) Address=14 PointAddress=10 not match"
"For object CCTVController(2022929596432) Address=14 PointAddress=10 not match"
"For object CCTVController(2022929596433) Address=15 PointAddress=10 not match"
"For object CCTVController(2022929596434) Address=16 PointAddress=10 not match"
"For object CCTVController(2022929596435) Address=18 PointAddress=10 not match"
"For object CCTVController(2022929596436) Address=19 PointAddress=10 not match"
"For object CCTVController(2022929596437) Address=20 PointAddress=10 not match"
"For object CCTVController(2022929596438) Address=21 PointAddress=10 not match"
"For object CCTVController(2022929596439) Address=22 PointAddress=10 not match"
"For object CCTVController(2022929596440) Address=23 PointAddress=10 not match"
"For object CCTVController(2022929596441) Address=24 PointAddress=10 not match"
"For object CCTVController(2022929596442) Address=25 PointAddress=10 not match"
"For object CCTVController(2022929596443) Address=26 PointAddress=10 not match"
"For object CCTVController(2022929596444) Address=27 PointAddress=10 not match"
"For object CCTVController(2022929596445) Address=101 PointAddress=0 not match"
"For object CCTVController(2022929596446) Address=102 PointAddress=0 not match"
"For object CCTVController(2022929596447) Address=103 PointAddress=0 not match"
"For object CCTVController(2022929596448) Address=104 PointAddress=0 not match"
"For object CCTVController(2022929596449) Address=105 PointAddress=0 not match"
"For object CCTVController(2022929596450) Address=106 PointAddress=0 not match"
"For object CCTVController(2022929596451) Address=107 PointAddress=0 not match"
"For object CCTVController(2022929596452) Address=108 PointAddress=0 not match"
"For object CCTVController(2022929596453) Address=109 PointAddress=0 not match"
"For object CCTVController(2022929596454) Address=110 PointAddress=0 not match"
"For object CCTVController(2022929596455) Address=111 PointAddress=0 not match"
"For object CCTVController(2022929596456) Address=112 PointAddress=0 not match"
"For object CCTVController(2022929596457) Address=113 PointAddress=0 not match"
"For object CCTVController(2022929596458) Address=114 PointAddress=0 not match"
"For object CCTVController(2022929596459) Address=115 PointAddress=0 not match"
"For object CCTVController(2022929596460) Address=116 PointAddress=0 not match"
"For object CCTVController(2022929596461) Address=117 PointAddress=0 not match"
"For object CCTVController(2022929596462) Address=118 PointAddress=0 not match"
"For object CCTVController(2022929596463) Address=119 PointAddress=0 not match"
"For object CCTVController(2022929596464) Address=120 PointAddress=0 not match"
"For object CCTVController(2022929596465) Address=121 PointAddress=0 not match"
"For object CCTVController(2022929596466) Address=122 PointAddress=0 not match"
"For object CCTVController(2022929596467) Address=123 PointAddress=0 not match"
"For object CCTVController(2022929596468) Address=124 PointAddress=0 not match"
"For object CCTVController(2022929596469) Address=125 PointAddress=0 not match"
"For object CCTVController(2022929596470) Address=126 PointAddress=0 not match"
"For object CCTVController(2022929596471) Address=127 PointAddress=0 not match"
"For object CCTVController(2022929596472) Address=128 PointAddress=0 not match"
"For object CCTVController(2022929596473) Address=129 PointAddress=0 not match"
"For object CCTVController(2022929596474) Address=130 PointAddress=0 not match"
"For object CCTVController(2022929596475) Address=131 PointAddress=0 not match"
"For object CCTVController(2022929596476) Address=132 PointAddress=0 not match"
"For object CCTVController(2022929596477) Address=133 PointAddress=0 not match"
"For object CCTVController(2022929596478) Address=134 PointAddress=0 not match"
"For object CCTVController(2022929596479) Address=135 PointAddress=0 not match"
"For object CCTVController(2022929596480) Address=136 PointAddress=0 not match"
"For object CCTVController(2022929596481) Address=137 PointAddress=0 not match"
"For object CCTVController(2022929596482) Address=138 PointAddress=0 not match"
"For object CCTVController(2022929596483) Address=139 PointAddress=0 not match"
"For object CCTVController(2022929596484) Address=140 PointAddress=0 not match"
"For object CCTVController(2022929596485) Address=141 PointAddress=0 not match"
"For object CCTVController(2022929596486) Address=142 PointAddress=0 not match"
"For object CCTVController(2022929596487) Address=143 PointAddress=0 not match"
"For object CCTVController(2022929596488) Address=144 PointAddress=0 not match"
"For object CCTVController(2022929596489) Address=145 PointAddress=0 not match"
"For object CCTVController(2022929596490) Address=146 PointAddress=0 not match"
"For object CCTVController(2022929596491) Address=147 PointAddress=0 not match"
"For object CCTVController(2022929596492) Address=148 PointAddress=0 not match"
"For object CCTVController(2022929596493) Address=149 PointAddress=0 not match"
"For object CCTVController(2022929596494) Address=150 PointAddress=0 not match"
"For object CCTVController(2022929596495) Address=151 PointAddress=0 not match"
"For object CCTVController(2022929596496) Address=152 PointAddress=0 not match"
"For object CCTVController(2022929596497) Address=153 PointAddress=0 not match"
"For object CCTVController(2022946373633) Address=1 PointAddress=10 not match"
"For object CCTVController(2023055425537) Address=28 PointAddress=10 not match"
"Total not matched objects count is 81"
log4cplus: Entering ConsoleAppender::close()..
log4cplus: Destroying appender named [STDOUT].
log4cplus: Destroying appender named [R].
log4cplus: Destroying appender named [R].
cleanup


*/
