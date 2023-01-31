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
    case DB_LONGLONG_DATA:
        d = INT64(*static_cast<LongLongData *>(ptr.data()));
        break;
    case DB_LINK_DATA:
        d = ObId(*static_cast<LinkData *>(ptr.data()));
        break;
    case DB_NAMES_DATA: break;
    case DB_OTYPE_DATA: break;
    case DB_STRING_DATA:
        d = QString::fromStdString(*static_cast<StringData *>(ptr.data()));
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
    , m_model_clear_every_time(true)
{
}

//select obid from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?
bool OMSQuery::selectObIdList(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    return selectObIdList(content);
}

bool OMSQuery::selectObIdList(const SqlContent &content)
{
    QVector<ObId> obidVect;
    ConditionVector condVect;
    clearModel();

    try {
        //condition
        for(const SqlAnd &value : content.sql_and) {
            if(!dataTypeIsValid(value.field.type)) {
                m_lastError =  ErrorDataType(value.field.type);
                return false;
            }
            AType and_key_atype = m_database->matchAType(value.field.name.toLocal8Bit().data());
            condVect.append(and_key_atype, stringToOperator(value.op), value.field.type, value.value.toString());
        }
        //find
        int count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());
        obidVect.resize(count);
        m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size(), obidVect.data(), obidVect.size());
    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    //fill model
    m_model->setHorizontalHeaderLabels({"obid"});
    for(const ObId &id : obidVect) {
        QStandardItem *item = new QStandardItem;
        item->setData(id, Qt::DisplayRole);
        m_model->appendRow({item});
    }

    return true;
}

//select StringData:Name, IntegerData:PointAddress from OMS where obid = ?
bool OMSQuery::selectAttributeByObId(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(sql, content)) {
        m_lastError = content.error;
        return false;
    }

    return selectAttributeByObId(content);
}

bool OMSQuery::selectAttributeByObId(const SqlContent &content)
{
    clearModel();

    QString obid = content.obid;

    //horizontalLabels
    QStringList horizontalLabels;
    for(int k = 0; k < content.sql_field.count(); k ++) {
        horizontalLabels << content.sql_field[k].name;
    }
    m_model->setHorizontalHeaderLabels(horizontalLabels);

    try {
        //fill model
        QList<QStandardItem *> itemList;
        for(int k = 0; k < content.sql_field.count(); k ++) {
            SqlField field = content.sql_field[k];
            if(!dataTypeIsValid(field.type)) {
                m_lastError = ErrorDataType(field.type);
                return false;
            }
            //read
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
    return exec(sql, QVariantList());
}

bool OMSQuery::exec(const QString &sql, const QVariantList &bindvalueList)
{
    Parser p;
    SqlContent content;
    if(!p.parse(sql, content, bindvalueList)) {
        m_lastError = content.error;
        return false;
    }
    switch(content.type) {
    case OMSQueryParser::SelectObIdList:
        return selectObIdList(content);
    case SelectCount:
        return selectCount(content);
    case SelectAttributeByObId:
        return selectAttributeByObId(content);
    case WriteAttribute:

        return writeAtrribute(content);
    case SelectAttribute:
        return selectAttribute(content);
    }
    return false;
}

bool OMSQuery::serialExec(const QString &sql)
{
    //backup model
    QVector<QVariantList> bindvalueRect(m_model->rowCount());
    for(int row = 0; row < m_model->rowCount(); row ++) {
        for(int col = 0; col < m_model->columnCount(); col ++) {
            bindvalueRect[row] << m_model->item(row, col)->data(Qt::DisplayRole);
        }
    }
    clearModel();

    //函数结束时确保m_model_clear_every_time=true
    ValueLocker<bool> locker(&m_model_clear_every_time, false, true);

    if(bindvalueRect.isEmpty())
        return exec(sql);

    for(int k = 0; k < bindvalueRect.count(); k ++) {
        if(!exec(sql, bindvalueRect[k])){
            return false;
        }
    }
    return true;
}

void OMSQuery::serialPrepare()
{
    clearModel();
}


//select count from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?
bool OMSQuery::selectCount(const QString &sql)
{
    Parser p;
    SqlContent content;
    if(!p.parse(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    return selectCount(content);
}

bool OMSQuery::selectCount(const SqlContent &content)
{
    ConditionVector condVect;
    int count = 0;
    clearModel();

    try {
        //condition
        for(const SqlAnd &value : content.sql_and) {
            if(!dataTypeIsValid(value.field.type)) {
                m_lastError =  ErrorDataType(value.field.type);
                return false;
            }
            AType and_key_atype = m_database->matchAType(value.field.name.toLocal8Bit().data());
            condVect.append(and_key_atype, stringToOperator(value.op), value.field.type, value.value.toString());
        }
        //find
        count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());

        //fill model
        QStandardItem *item = new QStandardItem;
        item->setData(count, Qt::DisplayRole);
        m_model->appendRow({item});

    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    return true;
}

//select obid, StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?
bool OMSQuery::selectAttribute(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    return selectAttribute(content);
}

bool OMSQuery::selectAttribute(const SqlContent &content)
{
    ConditionVector condVect;
    QVector<ObId> obidVect;
    clearModel();
    // select obid into obidVect
    try {
        //condition
        for(const SqlAnd &value : content.sql_and) {
            if(!dataTypeIsValid(value.field.type)) {
                m_lastError =  ErrorDataType(value.field.type);
                return false;
            }
            AType and_key_atype = m_database->matchAType(value.field.name.toLocal8Bit().data());
            condVect.append(and_key_atype, stringToOperator(value.op), value.field.type, value.value.toString());
        }

        //find
        int count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());
        obidVect.resize(count);
        m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size(), obidVect.data(), obidVect.size());
    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    //horizontalLables: obid, StringData:Name, IntegerData:PointAddress
    QStringList labels;
    for(const SqlField &f : content.sql_field)
        labels << f.name;
    m_model->setHorizontalHeaderLabels(labels);

    try {
        //file model
        for(const ObId &obid : obidVect) {
            //select field from oms where obid = ?
            QList<QStandardItem *> itemList;
            if(labels.contains(ObidName)) {
                itemList << new QStandardItem;
                itemList.last()->setData(obid, Qt::DisplayRole);
            }

            for(int k = 0; k < content.sql_field.count(); k ++) {
                SqlField field = content.sql_field[k];
                if(field.name == ObidName)
                    continue;
                if(!dataTypeIsValid(field.type)) {
                    m_lastError = ErrorDataType(field.type);
                    return false;
                }
                QSharedPointer<Data> dataPtr = createDataPtr(field.type);

                //read
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

//write into OMS (obid, StringData:Name) values(?, ?)
bool OMSQuery::writeAtrribute(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(sql, content)) {
        m_lastError = content.error;
        return false;
    }
    return writeAtrribute(content);
}

bool OMSQuery::writeAtrribute(const SqlContent &content)
{
    //option
    WriteOptions writeOptions = PUT_IF_NOT_EQUAL;
    if(!content.write_options.isEmpty()) {
        writeOptions = int(content.write_options.toInt());
    }

    QVector<Request> reqVector(content.sql_bind.size());
    QVector<QSharedPointer<Data> >fieldDataVector(content.sql_bind.size());

    try {
        //reqVector
        for(int k = 0; k < content.sql_bind.size(); k ++) {
            const SqlBind &bind = content.sql_bind[k];

            if(!dataTypeIsValid(bind.field.type)) {
                m_lastError = ErrorDataType(bind.field.type);
                return false;
            }

            QSharedPointer<Data> fieldData = createDataPtr(bind.field.type, bind.value);
            AType nAType = m_database->matchAType(bind.field.name.toLocal8Bit().data());
            ObId nObId = StringToObId(content.obid);

            fieldDataVector[k] = fieldData;
            reqVector[k].set(nObId,
                             nAType,
                             fieldData.data(),
                             NULL,
                             0,
                             writeOptions,
                             NULL);
        }
        //write
        m_database->write(reqVector.data(), reqVector.size());
    } catch(Exception &e) {
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

void OMSQuery::test()
{
    qDebug() << "test begin";
    //exec
    {
        OMSQuery query(m_database);
        query.exec("select obid from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("selectObIdList");
        query.exec("select obid from CCTVController where IntegerData:PointAddress = 10 and StringData:Address = 28");
        query.printModel("selectObIdList");

        query.exec("select count from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("selectCount");
        query.exec("select count from CCTVController where IntegerData:PointAddress = 10 and StringData:Address = 28");
        query.printModel("selectCount");

        query.exec("select StringData:Name, IntegerData:PointAddress from OMS where obid = ?",
                   QVariantList() << "2023055425537  ");
        query.printModel("selectAttributeByObId");
        query.exec("select StringData:Name, IntegerData:PointAddress from OMS where obid = 2023055425537");
        query.printModel("selectAttributeByObId");

        query.exec("select obid, StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("SelectAttribute");
        query.exec("select obid, StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = 10 and StringData:Address = 28");
        query.printModel("SelectAttribute");

        query.exec("select StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("SelectAttribute");
        query.exec("select StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = 10 and StringData:Address = 28");
        query.printModel("SelectAttribute");

        query.exec("select obid from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("SelectAttribute");
        query.exec("select obid from CCTVController where IntegerData:PointAddress = 10 and StringData:Address = 28");
        query.printModel("SelectAttribute");


        //test for write into
        //  select obid, Name
        query.exec("select obid, StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                   QVariantList() << 10 << 28);
        query.next();
        ObId obid = StringToObId(query.value("obid").toString());
        QString Name = query.value("Name").toString();
        //  482 "2023055425537" "马家园车辆段"
        //  chang Name
        Name += "(1)";
        //  write
        query.exec("write into OMS (obid, StringData:Name) values(?, ?)",
                   QVariantList() << obid << Name);
        //  read
        query.exec("select obid, StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                   QVariantList() << 10 << 28);
        query.next();
        //  check
        if(Name != query.value("Name").toString()) {
            qDebug() << "Error: test failed";
        } else {
            Name = query.value("Name").toString();
            Name.chop(3);
            //restore
            query.exec("write into OMS (obid, StringData:Name) values(?, ?)",
                       QVariantList() << obid << Name);
            query.exec("select obid, StringData:Name, IntegerData:PointAddress from CCTVController where IntegerData:PointAddress = ? and StringData:Address = ?",
                       QVariantList() << 10 << 28);
            query.next();
            qDebug() << __LINE__ << query.value("obid").toString() << query.value("Name").toString();
        }

    }

    //serialExec
    {
        OMSQuery query(m_database);

        query.exec("select obid, StringData:Name from ObjectType where StringData:Name = Camera");
        query.serialExec("select LongLongData:ObjectSpecifier,StringData:Name from ObjectAttribute "
                         "where StringData:Name = FaultState and LinkData:ParentLink=?");
        query.serialExec("select obid, StringData:Name, IntegerData:Rank from Choice where LinkData:ParentLink=?");
        query.printModel("serialExec");


        query.exec("select obid, StringData:Name from ObjectType where StringData:Name = ?", QVariantList() << "Camera");
        query.serialExec("select LongLongData:ObjectSpecifier,StringData:Name from ObjectAttribute "
                         "where StringData:Name = FaultState and LinkData:ParentLink=?");
        query.serialExec("select obid, StringData:Name, IntegerData:Rank from Choice where LinkData:ParentLink=?");
        query.printModel("serialExec");
    }
    qDebug() << "test end";
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
    } else if(type == "LongLongData") {
        d = QSharedPointer<Data>(new LongLongData);
    }
    return d;
}

QSharedPointer<Data> OMSQuery::createDataPtr(const QString &type, const QVariant &defaultValue)
{
    QSharedPointer<Data> d;
    if(type == "IntegerData") {
        d = QSharedPointer<Data>(new IntegerData(defaultValue.toInt()));
    } else if(type == "StringData") {
        d = QSharedPointer<Data>(new StringData(defaultValue.toString().toStdString()));
    } else if(type == "LinkData") {
        d = QSharedPointer<Data>(new LinkData(StringToObId(defaultValue.toString())));
    } else if(type == "FloatData") {
        d = QSharedPointer<Data>(new FloatData(defaultValue.toFloat()));
    } else if(type == "LongLongData") {
        d = QSharedPointer<Data>(new LongLongData(defaultValue.toLongLong()));
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

void OMSQuery::clearModel()
{
    if(!m_model_clear_every_time)
        return;
    m_model->removeRows(0, m_model->rowCount());
    m_model->removeColumns(0, m_model->columnCount());
    m_model_row = -1;
}

void OMSQuery::printModel(QString title)
{
    if(m_model->rowCount() == 0) {
        qDebug() << title <<  "Error: No Data!";
        return;
    }
    int width_cell = 13;
    int width_table = 1;
    for(int col = 0; col < m_model->columnCount(); col ++) {
        width_table += width_cell + 1;
    }
    width_table = qMax(width_table, 2 * (width_cell + 1) + 1);
    //title
    QString title_line;
    int width_title = (width_table - title.size() + 1)/2;
    title_line = QString(width_title, '-') + title + QString(width_title, '-');
    qDebug() << title_line;

    //head
    QString heads = "|";
    for(int col = 0; col < m_model->columnCount(); col ++) {
        heads += m_model->headerData(col, Qt::Horizontal).toString().leftJustified(width_cell) + "|";
    }
    qDebug() << heads;
    qDebug() << QString(width_table, '-');
    //content
    for(int row = 0; row < m_model->rowCount(); row ++) {
        QString line = "|";
        for(int col = 0; col < m_model->columnCount(); col ++) {
            line += m_model->item(row, col)->text().leftJustified(width_cell) + "|";
        }
        qDebug() << line;
    }
    qDebug() << QString(width_table, '-');
}

void OMSQuery::ConditionVector::append(AType aType, Comparison comparison, const QString &valueType, const QVariant &value)
{
    QSharedPointer<Data> d = createDataPtr(valueType, value);
    m_ptrVector << d;
    m_conditionVector.append(Condition(aType, comparison, d.data()));
}
