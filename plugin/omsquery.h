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

    /**
     * @brief selectObIdList
     * 一个简单的实时库ObId list查询方法
     */
    bool selectObIdList(const QString &sql);
    bool selectObIdList(const OMSQueryParser::SqlContent &content);

    /**
     * @brief selectAttributeByObId
     * 一个简单的实时库属性查询方法
     */
    bool selectAttributeByObId(const QString &sql);
    bool selectAttributeByObId(const OMSQueryParser::SqlContent &content);

    /**
     * @brief exec
     * 通用查询方法
     */
    bool exec(const QString &sql);
    bool exec(const QString &sql, const QVariantList &bindvalueList);

    /**
     * @brief serialExec
     * 支持多条sql连续查询，上次的结果集是这次的输入参数
     * 使用前推荐用serialPrepare来清理下model
     * exec = serialPrepare + serialExec
     */
    bool serialExec(const QString &sql);
    void serialPrepare();

    /**
     * @brief selectCount
     * 查询符合条件的数量
     */
    bool selectCount(const QString &sql);
    bool selectCount(const OMSQueryParser::SqlContent &content);

    /**
     * @brief selectAttribute
     * 一个包含了selectObIdList、selectAttribute功能的方法
     */
    bool selectAttribute(const QString &sql);
    bool selectAttribute(const OMSQueryParser::SqlContent &content);

    /**
     * @brief writeAtrribute
     * 写数据到OMS数据库里
     */
    bool writeAtrribute(const QString &sql);
    bool writeAtrribute(const OMSQueryParser::SqlContent &content);

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

    /**
     * @brief test
     * 测试
     */
    void test();
private:
    /**
     * @brief createDataPtr
     * 创建data的智能指针
     * @param type 如：IntegerData、StringData等
     */
    static QSharedPointer<Data> createDataPtr(const QString &type);
    static QSharedPointer<Data> createDataPtr(const QString &type, const QVariant &defaultValue);
    static bool dataTypeIsValid(const QString &type);

    /**
     * @brief The ConditionVector class
     * 简化Condition的使用，不用考虑指针释放问题
     */
    class ConditionVector {
    public:
        void append(AType aType, Comparison comparison, const QString &valueType, const QVariant &value);

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

    /**
     * @brief clearModel
     * 删除model的所有数据
     */
    void clearModel();

    /**
     * @brief printModel
     * 打印
     */
    void printModel(QString title);

    /**
     * @brief The ValueLocker class
     * 类QMutexLocker
     */
    template<typename T>
    class ValueLocker{
    public:
        ValueLocker(T *valuePtr, const T &constructionValue, const T &destoryValue){
            m_valuePtr      = valuePtr;
            *m_valuePtr     = constructionValue;
            m_destoryValue  = destoryValue;
        }

        ~ValueLocker(){
            *m_valuePtr = m_destoryValue;
        }

    private:
        T *m_valuePtr;
        T m_destoryValue;
    };

public:
    OMSDatabase *m_database;
    QString m_lastError;

    QStandardItemModel *m_model;
    int m_model_row;
    bool m_model_clear_every_time;
};

#endif // OMSQUERY_H
