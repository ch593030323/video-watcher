#ifndef OMSQUERY_H
#define OMSQUERY_H

#include "omsqueryparser.h"

#include <QSharedPointer>
#include <QVector>
#include <QVariant>
#include <QStringList>
#include <QStandardItemModel>

#include <oms/data_all.h>
#include <oms/qt_application.h>
#include <oms/database.h>
#include <oms/request.h>
#include <oms/oms_database.h>


/**
 * @brief The OMSQuery class
 * 一个类qsqlquery的oms database访问类
 */
class OMSQuery : public QObject
{
    Q_OBJECT

public:
    OMSQuery(OMSDatabase *database, QObject *parent = 0);

private:
    /**
     * @brief createDataPtr
     * 创建data的智能指针
     * @param type 如：IntegerData、StringData等
     */
    static QSharedPointer<Data> createDataPtr(const QString &type);
    static QSharedPointer<Data> createDataPtr(const QString &type, const QString &defaultValue);
    static bool dataTypeIsValid(const QString &type);

    /**
     * @brief The ConditionVector class
     * 简化Condition的使用，不用考虑指针释放问题
     */
    class ConditionVector {
    public:
        void append(AType aType, Comparison comparison, const QString &valueType, const QString &value);

        inline const Condition *constData() const { return m_conditionVector.data(); }
        inline int size() const {return m_conditionVector.size();}
    private:
        QVector<QSharedPointer<Data> > m_ptrVector;
        QVector<Condition> m_conditionVector;
    };

    /**
     * @brief stringToOpartor
     * 将操作符转为枚举，如: (op:=) >>> (Comparison:EQ)
     */
    Comparison stringToOperator(QString op);

public:
    /**
     * @brief selectObIdList
     * 一个简单的实时库ObId list查询方法
     */
    bool selectObIdList(const QString &sql);
    bool selectObIdList(const OMSQueryParser::SqlContent &content);

    /**
     * @brief selectAttribute
     * 一个简单的实时库属性查询方法
     */
    bool selectAttribute(const QString &sql);
    bool selectAttribute(const OMSQueryParser::SqlContent &content);

    /**
     * @brief select
     * 通用查询方法
     */
    bool exec(const QString &sql);

    /**
     * @brief selectCount
     * 查询符合条件的数量
     */
    bool selectCount(const QString &sql);
    bool selectCount(const OMSQueryParser::SqlContent &content);

    /**
     * @brief selectObidAndAttributeList
     * 一个包含了selectObIdList、selectAttribute功能的方法
     */
    bool selectObidAndAttributeList(const QString &sql);
    bool selectObidAndAttributeList(const OMSQueryParser::SqlContent &content);

    /**
    /* @brief lastError
     * 最后一次报错信息
     */
    inline QString lastError() const {return m_lastError;}

    /**
     * @brief next
     * 用于移动光标来读取更多的值，也可以判断select是否有效
     */
    bool next();

    /**
     * @brief value
     * next()后使用，读取select的值
     */
    QVariant value(int index);
    QVariant value(const QString &field_name);

private:
    OMSDatabase *m_database;
    QString m_lastError;

    QStandardItemModel *m_model;
    int m_model_row;
};

#endif // OMSQUERY_H
