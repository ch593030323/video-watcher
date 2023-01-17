#include "omsqueryparser.h"

#include <QtDebug>

static QVariant ifnull(const QVariant &p1, const QString &p2) {
    if(p1.isNull())
        return p2;
    return p1;
}

OMSQueryParser::Parser::Parser(QObject *parent) : QObject(parent)
{

}

bool OMSQueryParser::Parser::parse(const QString &sql, OMSQueryParser::SqlContent &content, const QVariantList &bindvalueList)
{
    BindRegexp rx(bindvalueList);
    int begin = 0;

    if(rx.indexInExecType(sql, SelectObIdList) >= 0) {
        content.type = SelectObIdList;

    } else if(rx.indexInExecType(sql, SelectCount) >= 0) {
        content.type = SelectCount;

    } else if(rx.indexInExecType(sql, SelectAttributeByObId) >= 0) {
        content.type = SelectAttributeByObId;

    } else if(rx.indexInExecType(sql, WriteAttribute) >= 0) {
        content.type = WriteAttribute;

    } else if(rx.indexInExecType(sql, SelectAttribute) >= 0) {
        content.type = SelectAttribute;

    } else {
        content.type = ExecType::None;
        content.error = ErrorQueryString(sql);
        return false;
    }

    switch(content.type) {
    case SelectCount:
    case SelectObIdList:
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
            QVariant and_value      = rx.cap(4);

            //where IntegerData:PointAddress = ? and StringData:Address = ?
            if(and_value == "?" && rx.bindNext())
                and_value = rx.bindValue();

            content.sql_and << SqlAnd{{and_key_type, and_key_name}, and_operator, and_value};
        }
    }
        break;
    case SelectAttributeByObId:
    {
        QString sql_string = rx.cap(0);
        QString sql_field  = rx.cap(1);
        QString sql_obid   = rx.cap(2);

        //where obid = ?
        if(sql_obid == "?" && rx.bindNext())
            sql_obid = rx.bindValue().toString();

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
    {
        QString sql_string          = rx.cap(0);
        QString sql_field           = rx.cap(1);
        QString sql_bind            = rx.cap(2);
        QString sql_write_options   = rx.cap(3);

        QStringList sql_field_list = sql_field.split(",", QString::SkipEmptyParts);
        QStringList sql_bind_list = sql_bind.split(",", QString::SkipEmptyParts);

        if(sql_field_list.isEmpty()) {
            content.error = ErrorQueryString(sql);
            return false;
        }
        if(sql_field_list.count() != sql_bind_list.count()) {
            content.error = ErrorQueryString(sql);
            return false;
        }
        //write into(obid, StringData:Name, IntegerData:PointAddress)
        rx.setPattern(rx_field);
        for(int k = 0; k < sql_field_list.count(); k ++) {
            //trimmed if field_value is ' ?'
            QVariant field_value = sql_bind_list.value(k).trimmed();

            //values(?,?,?)
            if(field_value == "?" && rx.bindNext())
                field_value = rx.bindValue();

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

                content.sql_bind << SqlBind{{field_key, field_name}, field_value};
            }
        }
        //write option
        content.write_options = sql_write_options;
        if(content.write_options == "?" && rx.bindNext())
            content.write_options = rx.bindValue().toString();

        //  write_option must be digit
        bool numberOk = false;
        content.write_options.toInt(&numberOk);
        if(!content.write_options.isEmpty() && !numberOk) {
            content.error = ErrorQueryStringMsg("writeOptions must be digit!", sql);
            return false;
        }
    }
        break;
    case SelectAttribute:
    {
        QString sql_string          = rx.cap(0);
        QString sql_field           = rx.cap(1);
        QString sql_table           = rx.cap(2);
        QString sql_and             = rx.cap(3);

        if(sql_field.startsWith(ObidName)) {
            sql_field.remove(0, ObidName.length());
            content.sql_field << SqlField{"", ObidName};
        }

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
            QVariant and_value      = rx.cap(4);

            //where IntegerData:PointAddress = ? and StringData:Address = ?
            if(and_value == "?" && rx.bindNext())
                and_value = rx.bindValue();

            content.sql_and << SqlAnd{{and_key_type, and_key_name}, and_operator, and_value};
        }
        //
    }
        break;
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
    m_bindvalueIndex = -1;
}

int OMSQueryParser::BindRegexp::indexInExecType(const QString &str, OMSQueryParser::ExecType type)
{
    QString rx_string  = regexpSql(type);
    setPattern(rx_string);
    int r = indexIn(str);
    return r;
}

bool OMSQueryParser::BindRegexp::bindNext()
{
    m_bindvalueIndex++;
    return m_bindvalueIndex < m_bindvalueList.count();
}

QVariant OMSQueryParser::BindRegexp::bindValue()
{
    return m_bindvalueList.value(m_bindvalueIndex);
}

QString OMSQueryParser::regexpSql(OMSQueryParser::ExecType type)
{
    switch(type) {
    case SelectObIdList:
        return R"(select\s+obid\s+from\s+(\w+))" + rx_where;
    case SelectAttributeByObId:
        return QString(R"(select\s+(\w+\s*:\s*\w+(?:\s*,\s*\w+\s*:\s*\w+)*)\s+from\s+OMS\s+where\s+obid\s*=\s*(\?))")
                .replace(R"(\?)", rx_bindvalue);
    case WriteAttribute:
        return QString(R"(write\s+into\s+OMS\s*\((\s*obid(?:\s*,\s*\w+\s*:\s*\w+)*)\)\s*values\s*\((\s*\?(?:\s*,\s*\?)*)\))" + rx_option)
                .replace(R"(\?)", rx_bindvalue);
    case SelectAttribute:
        return R"(select\s+((?:obid|\w+\s*:\s*\w+)?(?:\s*,\s*\w+\s*:\s*\w+)*)\s+from\s+(\w+))" + rx_where;
    case SelectCount:
        return R"(select\s+count\s+from\s+(\w+))" + rx_where;
    }
    return "";
}
