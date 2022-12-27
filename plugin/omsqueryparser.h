#ifndef OMSQUERYPARSER_H
#define OMSQUERYPARSER_H

#include <QObject>
#include <QVariant>
#include <QRegExp>
#include <QStringList>
#include <QtDebug>

namespace OMSQueryParser {
#define StringToObId(string)        string.toULongLong()
#define ErrorQueryString(sql)       ("Error: query '" + sql + "' cannot be resolved.")
#define ErrorDataType(dataType)     ("Error: data type'" + dataType + "' cannot be resolved.")


/**
 * @brief The ExecType enum
 * 查询语句的类型
 */
enum ExecType{
    None,
    SelectObIdList,
    SelectCount,
    SelectAttribute,
    WriteAttribute,
    SelectObidAndAttributeList
};

/**
 * @brief The SqlField struct
 * 查询语句的结果字段，如select string:name
 */
struct SqlField {
    QString type;
    QString name;
};

/**
 * @brief The SqlAnd struct
 * 查询语句的条件字段,如 from string:name=hello
 */
struct SqlAnd {
    SqlField field;
    QString op;
    QVariant value;
};

/**
 * @brief The SqlBind struct
 * write into 用到，如 write into table (string:name) values(hello)
 */
struct SqlBind {
    SqlField field;
    QVariant value;
};

/**
 * @brief The SqlContent struct
 * 一次解析sql的结果集
 */
struct SqlContent {
    ///SelectObIdList
    QString table;
    QList<SqlAnd> sql_and;

    ///SelectAttribute
    QList<SqlField> sql_field;
    QString obid;

    ///WriteAttribute
    QList<SqlBind> sql_bind;
    //        QString obid;
    QString write_options;

    ///SelectObidAndAttributeList
    //    QString table;
    //    QList<SqlAnd> sql_and;
    //    QList<SqlField> sql_field;

    ///SelectCount
    //    QString table;
    //    QList<SqlAnd> sql_and;

    ExecType type;
    QString error;
    //
    void print();
};

//
static const QString rx_operator = R"((?:=|!=|<|<=|>|>=){1})";
static const QString rx_and_or   = R"((?:and|or){1})";
static const QString rx_option   = R"((?:\s+writeOptions\s+(\w+))?)";
static const QString rx_field    = R"((\w+)\s*:\s*(\w+))";

//where IntegerData:PointAdress = ? and StringData:Adress = ?
static const QString rx_where    = QString(R"(\s+where\s+(\w+\s*:\s*\w+\s*=\s*\?(?:\s+and\s+\w+\s*:\s*\w+\s*=\s*\?)*))")
        .replace("=", rx_operator);

//StringData:Adress = ?
static const QString rx_and      = QString(R"((\w+)\s*:\s*(\w+)\s*(=)\s*(\?))")
        .replace("=", rx_operator);

/**
 * @brief regexpSql
 * 根据type返回对应的正则表达式
 */
static QString regexpSql(ExecType type);

/**
 * @brief The BindRegexp struct
 * 当bindvalueList有效时，会优先读取bindvalueList的值
 */
struct BindRegexp : QRegExp {
    BindRegexp(const QVariantList &bindvalueList);

    void setPattern(const QString &pattern);

    /**
     * @brief indexInExecType
     * 执行两个操作，setPattern 和 indexIn
     */
    int indexInExecType(const QString &str, ExecType type);

    QString cap(int nth);
    QVariant cap(int nth, int bindedIndex);

private:
    QVariantList m_bindvalueList;
};

/**
 * @brief The Parser class
 * sql的解析类
 */
class Parser : public QObject {
public:
    Parser(QObject *parent = 0);

    bool parser(const QString &sql, SqlContent &content, const QVariantList &bindvalueList = QVariantList());
    //
    void prepare(const QString &sql);

    void addBindValue(int index, const QVariant &var);

    bool exec(SqlContent &content);
    //
    bool exec(SqlContent &content, const QString &sql);

private:
    QString      m_prepareSql;
    QVariantList m_bindvalueList;
};
}

#endif // OMSQUERYPARSER_H
