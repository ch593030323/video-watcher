#include "omsqueryparser.h"

static QVariant ifnull(const QVariant &p1, const QString &p2) {
    if(p1.isNull())
        return p2;
    return p1;
}

OMSQueryParser::Parser::Parser(QObject *parent) : QObject(parent)
{

}

bool OMSQueryParser::Parser::parser(const QString &sql, OMSQueryParser::SqlContent &content, const QVariantList &bindvalueList)
{
    BindRegexp rx(bindvalueList);
    int begin = 0;

    if(rx.indexInExecType(sql, SelectObIdList) >= 0) {
        content.type = SelectObIdList;
    } else if(rx.indexInExecType(sql, SelectCount) >= 0) {
        content.type = SelectCount;

    } else if(rx.indexInExecType(sql, SelectAttribute) >= 0) {
        content.type = SelectAttribute;

    } else if(rx.indexInExecType(sql, WriteAttribute) >= 0) {
        content.type = WriteAttribute;

    } else if(rx.indexInExecType(sql, SelectObidAndAttributeList) >= 0) {
        content.type = SelectObidAndAttributeList;

    } else {
        content.type = ExecType::None;
        content.error = ErrorQueryString(sql);
        return false;
    }

    switch(content.type) {
    case SelectCount:
        // select count from CCTVController where IntegerData:PointAdress = ? and StringData:Adress = ?
    case SelectObIdList:
        // select obid from CCTVController where IntegerData:PointAdress = ? and StringData:Adress = ?
    {
        QString sql_string          = rx.cap(0);
        QString sql_table           = rx.cap(1);
        QString sql_and             = rx.cap(2);

        content.table = sql_table;
        rx.setPattern(rx_and);
        QStringList sql_and_list = sql_and.split("and", QString::SkipEmptyParts, Qt::CaseInsensitive);
        for(int k = 0; k < sql_and_list.count(); k ++) {
            begin = rx.indexIn(sql_and_list[k]);
            if(begin < 0) {
                content.error = ErrorQueryString(sql);
                return false;
            }

            QString and_string      = rx.cap(0);
            QString and_key_type    = rx.cap(1);
            QString and_key_name    = rx.cap(2);
            QString and_operator    = rx.cap(3);
            QVariant and_value      = rx.cap(4, k);
            content.sql_and << SqlAnd{SqlField{and_key_type, and_key_name}, and_operator, and_value};
        }
    }
        break;
    case SelectAttribute:
        // select StringData:Name, IntegerData:PointAdress from OMS where obid = ?
    {
        QString sql_string = rx.cap(0);
        QString sql_field  = rx.cap(1);
        QString sql_obid   = rx.cap(2, 0).toString();
        content.obid = sql_obid;

        QStringList sql_field_list = sql_field.split(",", QString::SkipEmptyParts, Qt::CaseInsensitive);
        for(int k = 0; k < sql_field_list.count(); k ++) {
            rx.setPattern(rx_field);
            begin = rx.indexIn(sql_field_list[k]);
            if(begin < 0) {
                content.error = ErrorQueryString(sql);
                return false;
            }

            QString field_string = rx.cap(0);
            QString field_type   = rx.cap(1);
            QString field_name   = rx.cap(2);
            content.sql_field << SqlField{field_type, field_name};
        }
    }
        break;
    case WriteAttribute:
        // write into OMS (obid, StringData:Name, IntegerData:PointAdress) values (?, ?, ?) writeOptions 23
    {
        QString sql_string          = rx.cap(0);
        QString sql_field           = rx.cap(1);
        QString sql_bind            = rx.cap(2);
        QString sql_write_options   = rx.cap(3);

        QStringList sql_field_list = sql_field.split(",", QString::SkipEmptyParts);
        QStringList sql_bind_list = sql_bind.split(",", QString::SkipEmptyParts);

        content.write_options = sql_write_options;
        if(sql_field_list.isEmpty()) {
            content.error = ErrorQueryString(sql);
            return false;
        }
        if(sql_field_list.count() != sql_bind_list.count()) {
            content.error = ErrorQueryString(sql);
            return false;
        }
        //obid, StringData:Name, IntegerData:PointAdress
        rx.setPattern(rx_field);
        for(int k = 0; k < sql_field_list.count(); k ++) {
            QVariant field_value = ifnull(bindvalueList.value(k), sql_bind_list.value(k));
            if(k == 0) {
                content.obid = field_value.toString();
            } else {
                begin = rx.indexIn(sql_field_list[k]);
                if(begin < 0) {
                    content.error = ErrorQueryString(sql);
                    return false;
                }
                QString field_string    = rx.cap(0);
                QString field_key       = rx.cap(1);
                QString field_name      = rx.cap(2);

                content.sql_bind << SqlBind{field_key, field_name, field_value};
            }
        }
    }
        break;
    case SelectObidAndAttributeList:
        // select obid, StringData:Name, IntegerData:PointAdress from CCTVController where IntegerData:PointAdress = ? and StringData:Adress = ?
    {
        QString sql_string          = rx.cap(0);
        QString sql_field           = rx.cap(1);
        QString sql_table           = rx.cap(2);
        QString sql_and             = rx.cap(3);

        content.table = sql_table;
        //
        QStringList sql_field_list = sql_field.split(",", QString::SkipEmptyParts, Qt::CaseInsensitive);
        for(int k = 0; k < sql_field_list.count(); k ++) {
            rx.setPattern(rx_field);
            begin = rx.indexIn(sql_field_list[k]);
            if(begin < 0) {
                content.error = ErrorQueryString(sql);
                return false;
            }

            QString field_string = rx.cap(0);
            QString field_type   = rx.cap(1);
            QString field_name   = rx.cap(2);
            content.sql_field << SqlField{field_type, field_name};
        }

        //
        rx.setPattern(rx_and);
        QStringList sql_and_list = sql_and.split("and", QString::SkipEmptyParts, Qt::CaseInsensitive);
        for(int k = 0; k < sql_and_list.count(); k ++) {
            begin = rx.indexIn(sql_and_list[k]);
            if(begin < 0) {
                content.error = ErrorQueryString(sql);
                return false;
            }

            QString and_string      = rx.cap(0);
            QString and_key_type    = rx.cap(1);
            QString and_key_name    = rx.cap(2);
            QString and_operator    = rx.cap(3);
            QVariant and_value      = rx.cap(4, k);
            content.sql_and << SqlAnd{SqlField{and_key_type, and_key_name}, and_operator, and_value};
        }
        //
    }
        break;
    }
    return true;
}

void OMSQueryParser::Parser::prepare(const QString &sql)
{
    m_prepareSql = sql;
    m_bindvalueList.clear();
}

void OMSQueryParser::Parser::addBindValue(int index, const QVariant &var)
{
    m_bindvalueList << var;
}

bool OMSQueryParser::Parser::exec(SqlContent &content)
{
    if(!parser(m_prepareSql, content, m_bindvalueList)) {
        qDebug() << content.error;
        return false;
    }
    return true;
}

bool OMSQueryParser::Parser::exec(SqlContent &content, const QString &sql)
{
    if(!parser(sql, content)) {
        qDebug() << content.error;
        return false;
    }
    return true;
}

void OMSQueryParser::SqlContent::print()
{
    qDebug() << "type:" << type;
    qDebug() << "table:" << table;
    qDebug() << "obid:" << obid;
    qDebug() << "write_options:" << write_options;
    for(SqlAnd value : sql_and) {
        qDebug() <<  "sql_and:" << value.field.type << value.field.name << value.op << value.value;
    }
    for(SqlField value : sql_field) {
        qDebug() << "sql_field:" << value.type << value.name;
    }
    for(SqlBind value : sql_bind) {
        qDebug() << "sql_bind:" << value.field.type << value.field.name << value.value;
    }
}

OMSQueryParser::BindRegexp::BindRegexp(const QVariantList &bindvalueList)
{
    setCaseSensitivity(Qt::CaseInsensitive);
    m_bindvalueList = bindvalueList;
}

void OMSQueryParser::BindRegexp::setPattern(const QString &pattern)
{
    if(m_bindvalueList.isEmpty()) {
        QString p = pattern;
        QRegExp::setPattern(p.replace(R"(\?)", R"(\w+)"));
    } else {
        QRegExp::setPattern(pattern);
    }
}

int OMSQueryParser::BindRegexp::indexInExecType(const QString &str, OMSQueryParser::ExecType type)
{
    QString rx_string = regexpSql(type);
    setPattern(rx_string);
    int r = indexIn(str);
    return r;
}

QString OMSQueryParser::BindRegexp::cap(int nth)
{
    return QRegExp::cap(nth);
}

QVariant OMSQueryParser::BindRegexp::cap(int nth, int bindedIndex)
{
    return ifnull(m_bindvalueList.value(bindedIndex), QRegExp::cap(nth));
}


QString OMSQueryParser::regexpSql(OMSQueryParser::ExecType type)
{
    switch(type) {
    case SelectObIdList:
        return R"(select\s+obid\s+from\s+(\w+))" + rx_where;
    case SelectAttribute:
        return  R"(select\s+(\w+\s*:\s*\w+(?:\s*,\s*\w+\s*:\s*\w+)*)\s+from\s+OMS\s+where\s+obid\s*=\s*(\?))";
    case WriteAttribute:
        return R"(write\s+into\s+OMS\s*\((\s*obid(?:\s*,\s*\w+\s*:\s*\w+)*)\)\s*values\s*\((\s*\?(?:\s*,\s*\?)*)\))" + rx_option;
    case SelectObidAndAttributeList:
        return  R"(select\s+obid((?:\s*,\s*\w+\s*:\s*\w+)*)\s+from\s+(\w+))" + rx_where;
    case SelectCount:
        return R"(select\s+count\s+from\s+(\w+))" + rx_where;
    }
    return "";
}
