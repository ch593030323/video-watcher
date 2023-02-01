#ifndef OMSQUERYPARSER_H
#define OMSQUERYPARSER_H

#include <QObject>
#include <QVariant>
#include <QRegExp>
#include <QStringList>

namespace OMSQueryParser {

#define StringToObId(string)            string.toULongLong()
#define ErrorQueryString(sql)           (QString() + "OMSQueryParser Error: query '" + sql + "' cannot be resolved.")
#define ErrorQueryStringMsg(msg, sql)   (QString() + "OMSQueryParser Error: " + msg + " In query '" + sql + "' cannot be resolved.")
#define ErrorDataType(dataType)         (QString() + "OMSQueryParser Error: data type '" + dataType + "' cannot be resolved.")

/**
 * @brief The ExecType enum
 * 查询语句的类型
 */
enum ExecType{
    None,

    // select obid from CCTVController where PointAddress = ? and Address = ?
    SelectObIdList,

    // select count from CCTVController where PointAddress = ? and Address = ?
    SelectCount,

    // select Name, PointAddress from OMS where obid = ?
    SelectAttributeByObId,

    // write into OMS (obid, Name, PointAddress) values (?, ?, ?) writeOptions 23
    WriteAttribute,

    // select obid, Name, PointAddress from CCTVController where PointAddress = ? and Address = ?
    // select obid                                           from CCTVController
    // select Name, PointAddress       from CCTVController where PointAddress = ? and Address = ?
    SelectAttribute,
};

/**
 * @brief ObidName
 * 查询语句中出现的关键字obid
 */
static const QString ObidName = "obid";

/**
 * @brief The SqlAnd struct
 * 查询语句的条件字段,如 from field_name=hello
 */
struct SqlAnd {
    QByteArray field_name;
    QString op;
    QVariant value;
};

/**
 * @brief The SqlBind struct
 * write into 用到，如 write into table (field_name) values(value)
 */
struct SqlBind {
    QByteArray field_name;
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

    ///SelectAttributeByObId
    QList<QByteArray> sql_field_list;
    QString obid;

    ///WriteAttribute
    QList<SqlBind> sql_bind;
    //        QString obid;
    QString write_options;

    ///SelectAttribute
    //    QString table;
    //    QList<SqlAnd> sql_and;
    //    QList<QByteArray> sql_field_list;

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
static const QString rx_option   = R"((?:\s+writeOptions\s+(\?))?)";
static const QString rx_field    = R"((\w+))";
static const QString rx_bindvalue= R"((?:\?|\w+))";

//where PointAddress = ? and Address = ?
static const QString rx_where_d  = QString(R"(\s+where\s+(\w+\s*=\s*\?(?:\s+and\s+\w+\s*=\s*\?)*))")
        .replace("=", rx_operator)
        .replace(R"(\?)", rx_bindvalue);
static const QString rx_where    = "(?:" + rx_where_d + ")?";

//Address = ? and xx = yy
static const QString rx_and      = QString(R"((\w+)\s*(=)\s*(\?))")
        .replace("=", rx_operator)
        .replace(R"(\?)", rx_bindvalue);

/**
 * @brief regexpSql
 * 根据type返回对应的正则表达式
 */
QString regexpSql(ExecType type);

/**
 * @brief The BindRegexp struct
 * 当bindvalueList有效时，会优先读取bindvalueList的值
 */
struct BindRegexp : QRegExp {
    BindRegexp(const QVariantList &bindvalueList);

    /**
     * @brief indexInExecType
     * 执行两个操作，setPattern 和 indexIn
     */
    int indexInExecType(const QString &str, OMSQueryParser::ExecType type);

    /**
     * @brief bindNext
     * 遍历bindvalueList
     */
    bool bindNext();

    /**
     * @brief bindValue
     * 通常在next()后调用，读取bindvalueList的值
     */
    QVariant bindValue();

private:
    QVariantList m_bindvalueList;
    int m_bindvalueIndex;
};

/**
 * @brief The Parser class
 * sql的解析类
 */
class Parser : public QObject {
public:
    Parser(QObject *parent = 0);

    bool parse(const QString &sql, SqlContent &content, const QVariantList &bindvalueList = QVariantList());
};
}

#endif // OMSQUERYPARSER_H
