#include "omsquery.h"
#include <QtDebug>


using namespace OMSQueryParser;

//TODO some type not be converted
static QVariant dataPtrToVariant(QSharedPointer<Data> ptr) {

    if(ptr.isNull()) {
        return QVariant();
    }

    QVariant d;
    switch(ptr->getDBDataType()) {
    case DB_ATYPE_DATA: break;
    case DB_ATYPE_LIST_DATA: break;
    case DB_CHOICE_DATA: break;
    case DB_CHOICE_LINK_DATA: break;
    case DB_COMPLETE_CONTAINER_DATA: break;
    case DB_COMPLETE_FLOAT_DATA: break;
    case DB_COMPLETE_INTEGER_DATA: break;
    case DB_COMPLETE_LONGLONG_DATA: break;
    case DB_COMPLETE_LINK_DATA: break;
    case DB_CONTAINER_DATA: break;
    case DB_DATA: break;
    case DB_FILE_DATA: break;
    case DB_FLOAT_DATA:
        d = float(*static_cast<FloatData *>(ptr.data()));
        break;
    case DB_INTEGER_DATA:
        d = int(*static_cast<IntegerData *>(ptr.data()));
        break;
    case DB_LONGLONG_DATA: break;
    case DB_LINK_DATA:
        d = ObId(*static_cast<LinkData *>(ptr.data()));
        break;
    case DB_NAMES_DATA: break;
    case DB_OTYPE_DATA: break;
    case DB_STRING_DATA:
        d = QString::fromStdString(*static_cast<LinkData *>(ptr.data()));
        break;
    case DB_TEXT_DATA: break;
    case DB_TEXT_FILE_DATA: break;
    case DB_TIME_DATA: break;
    case DB_TIME_INTERVAL_DATA: break;
    }

    return d;
}

OMSQuery::OMSQuery(OMSDatabase *database, QObject *parent)
    : QObject(parent)
    , m_database(database)
    , m_model(new QStandardItemModel(this))
    , m_model_row(-1)
{
}

//select obid from CCTVController where IntegerData:PointAdress = ? and StringData:Adress = ?
bool OMSQuery::selectObIdList(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parser(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    return selectObIdList(content);
}

bool OMSQuery::selectObIdList(const SqlContent &content)
{
    QVector<ObId> obidVect;
    ConditionVector condVect;
    m_model->removeRows(0, m_model->rowCount());
    m_model_row = -1;

    try {
        for(const SqlAnd &value : content.sql_and) {
            if(!dataTypeIsValid(value.field.type)) {
                m_lastError =  ErrorDataType(value.field.type);
                return false;
            }
            AType and_key_atype = m_database->matchAType(value.field.name.toLocal8Bit().data());
            condVect.append(and_key_atype, stringToOperator(value.op), value.field.type, value.value.toString());
        }
        int count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());
        obidVect.resize(count);
        m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size(), obidVect.data(), obidVect.size());
    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    m_model->setHorizontalHeaderLabels({"obid"});
    for(const ObId &id : obidVect) {
        QStandardItem *item = new QStandardItem;
        item->setData(id, Qt::DisplayRole);
        m_model->appendRow({item});
    }

    return true;
}

//select StringData:Name, IntegerData:PointAdress from OMS where obid = ?
bool OMSQuery::selectAttribute(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parser(sql, content)) {
        m_lastError = content.error;
        return false;
    }

    return selectAttribute(content);
}

bool OMSQuery::selectAttribute(const SqlContent &content)
{
    m_model->removeRows(0, m_model->rowCount());
    m_model_row = -1;

    QString obid = content.obid;

    //horizontalLabels
    QStringList horizontalLabels;
    for(int k = 0; k < content.sql_field.count(); k ++) {
        horizontalLabels << content.sql_field[k].name;
    }
    m_model->setHorizontalHeaderLabels(horizontalLabels);

    try {
        //model fill items
        QList<QStandardItem *> itemList;
        for(int k = 0; k < content.sql_field.count(); k ++) {
            SqlField field = content.sql_field[k];
            if(!dataTypeIsValid(field.type)) {
                m_lastError = ErrorDataType(field.type);
                return false;
            }
            QSharedPointer<Data> dataPtr = createDataPtr(field.type);
            AType att_atype = m_database->matchAType(field.name.toLocal8Bit().data());
            Request req(StringToObId(obid), att_atype, dataPtr.data());
            m_database->read(&req, 1);

            itemList << new QStandardItem();
            itemList.last()->setData(dataPtrToVariant(dataPtr), Qt::DisplayRole);
        }
        m_model->appendRow(itemList);
    } catch (Exception &e){
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }
    return true;
}

bool OMSQuery::exec(const QString &sql)
{
    Parser p;
    SqlContent content;
    if(!p.parser(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    switch(content.type) {
    case OMSQueryParser::SelectObIdList:
        return selectObIdList(content);
    case SelectCount:
        return selectCount(content);
    case SelectAttribute:
        return selectAttribute(content);
    case WriteAttribute:
        //TODO write
        return false;
    case SelectObidAndAttributeList:
        return selectObidAndAttributeList(content);
    }
    return false;
}


//select count from CCTVController where IntegerData:PointAdress = ? and StringData:Adress = ?
bool OMSQuery::selectCount(const QString &sql)
{
    Parser p;
    SqlContent content;
    if(!p.parser(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    return selectCount(content);
}

bool OMSQuery::selectCount(const SqlContent &content)
{
    ConditionVector condVect;
    int count = 0;
    m_model->removeRows(0, m_model->rowCount());
    m_model_row = -1;

    try {
        for(const SqlAnd &value : content.sql_and) {
            if(!dataTypeIsValid(value.field.type)) {
                m_lastError =  ErrorDataType(value.field.type);
                return false;
            }
            AType and_key_atype = m_database->matchAType(value.field.name.toLocal8Bit().data());
            condVect.append(and_key_atype, stringToOperator(value.op), value.field.type, value.value.toString());
        }
        count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());

        QStandardItem *item = new QStandardItem;
        item->setData(count, Qt::DisplayRole);
        m_model->appendRow({item});

    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    return true;
}

//select obid, StringData:Name, IntegerData:PointAdress from CCTVController where IntegerData:PointAdress = ? and StringData:Adress = ?
bool OMSQuery::selectObidAndAttributeList(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parser(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    return selectObidAndAttributeList(content);
}

bool OMSQuery::selectObidAndAttributeList(const SqlContent &content)
{
    ConditionVector condVect;
    QVector<ObId> obidVect;
    m_model->removeRows(0, m_model->rowCount());
    m_model_row = -1;

    try {
        // select obid into obidVect
        for(const SqlAnd &value : content.sql_and) {
            if(!dataTypeIsValid(value.field.type)) {
                m_lastError =  ErrorDataType(value.field.type);
                return false;
            }
            AType and_key_atype = m_database->matchAType(value.field.name.toLocal8Bit().data());
            condVect.append(and_key_atype, stringToOperator(value.op), value.field.type, value.value.toString());
        }

        int count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());
        obidVect.resize(count);
        m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size(), obidVect.data(), obidVect.size());
    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    //horizontalLables: obid, StringData:Name, IntegerData:PointAdress
    QStringList labels;
    labels << "obid";
    for(const SqlField &f : content.sql_field)
        labels << f.name;
    m_model->setHorizontalHeaderLabels(labels);

    try {
        //file model items
        for(const ObId &obid : obidVect) {
            //select field from oms where obid = ?
            QList<QStandardItem *> itemList;
            itemList << new QStandardItem;
            itemList.last()->setData(obid, Qt::DisplayRole);

            for(int k = 0; k < content.sql_field.count(); k ++) {
                SqlField field = content.sql_field[k];
                if(!dataTypeIsValid(field.type)) {
                    m_lastError = ErrorDataType(field.type);
                    return false;
                }
                QSharedPointer<Data> dataPtr = createDataPtr(field.type);

                AType att_atype = m_database->matchAType(field.name.toLocal8Bit().data());
                Request req(obid, att_atype, dataPtr.data());
                m_database->read(&req, 1);

                itemList << new QStandardItem();
                itemList.last()->setData(dataPtrToVariant(dataPtr), Qt::DisplayRole);
            }//~for
            m_model->appendRow(itemList);
        }//~for
    } catch (Exception &e){
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }
    return true;
}

bool OMSQuery::next()
{
    m_model_row ++;
    return m_model_row < m_model->rowCount();
}

QVariant OMSQuery::value(int index)
{
    QStandardItem *item = m_model->item(m_model_row, index);
    return item? item->data(Qt::DisplayRole) : QVariant();
}

QVariant OMSQuery::value(const QString &field_name)
{
    for(int k = 0; k < m_model->columnCount(); k ++) {
        if(m_model->headerData(k, Qt::Horizontal).toString() == field_name) {
            return value(k);
        }
    }
    return QVariant();
}

QSharedPointer<Data> OMSQuery::createDataPtr(const QString &type)
{
    QSharedPointer<Data> d;
    if(type == "IntegerData") {
        d = QSharedPointer<Data>(new IntegerData);
    } else if(type == "StringData") {
        d = QSharedPointer<Data>(new StringData);
    } else if(type == "LinkData") {
        d = QSharedPointer<Data>(new LinkData);
    } else if(type == "FloatData") {
        d = QSharedPointer<Data>(new FloatData);
    }
    return d;
}

QSharedPointer<Data> OMSQuery::createDataPtr(const QString &type, const QString &defaultValue)
{
    QSharedPointer<Data> d;
    if(type == "IntegerData") {
        d = QSharedPointer<Data>(new IntegerData(defaultValue.toInt()));
    } else if(type == "StringData") {
        d = QSharedPointer<Data>(new StringData(defaultValue.toStdString()));
    } else if(type == "LinkData") {
        d = QSharedPointer<Data>(new LinkData(StringToObId(defaultValue)));
    } else if(type == "FloatData") {
        d = QSharedPointer<Data>(new FloatData(defaultValue.toFloat()));
    }
    return d;
}

bool OMSQuery::dataTypeIsValid(const QString &type)
{
    return !createDataPtr(type).isNull();
}

Comparison OMSQuery::stringToOperator(QString op)
{
    //=|!=|<|<=|>|>=
    if("=" == op) {
        return EQ;
    }
    if("!=" == op) {
        return NE;
    }
    if("<" == op) {
        return LT;
    }
    if("<=" == op) {
        return LE;
    }
    if(">" == op) {
        return GT;
    }
    if(">=" == op) {
        return GE;
    }
    return INVALID_COMPARISON;
}

void OMSQuery::ConditionVector::append(AType aType, Comparison comparison, const QString &valueType, const QString &value)
{
    QSharedPointer<Data> d = createDataPtr(valueType, value);
    m_ptrVector << d;
    m_conditionVector.append(Condition(aType, comparison, d.data()));
}
