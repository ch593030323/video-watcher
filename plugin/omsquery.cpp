#include "omsquery.h"
#include <QtDebug>


using namespace OMSQueryParser;

//TODO some type not be converted
/**
 * @brief DataPtrToVariant
 * Data转QVariant
 */
static QVariant DataPtrToVariant(const Data *ptr) {
    switch(ptr->getDBDataType()) {
    case DB_ATYPE_DATA:                 break;
    case DB_ATYPE_LIST_DATA:            break;
    case DB_CHOICE_DATA:                break;
    case DB_CHOICE_LINK_DATA:           break;
    case DB_COMPLETE_CONTAINER_DATA:    break;
    case DB_COMPLETE_FLOAT_DATA:        break;
    case DB_COMPLETE_INTEGER_DATA:      break;
    case DB_COMPLETE_LONGLONG_DATA:     break;
    case DB_COMPLETE_LINK_DATA:         break;
    case DB_CONTAINER_DATA:             break;
    case DB_DATA:                       break;
    case DB_FILE_DATA:                  break;

    case DB_FLOAT_DATA:                 return float(*static_cast<const FloatData *>(ptr));
    case DB_INTEGER_DATA:               return int(*static_cast<const IntegerData *>(ptr));
    case DB_LONGLONG_DATA:              return INT64(*static_cast<const LongLongData *>(ptr));
    case DB_LINK_DATA:                  return ObId(*static_cast<const LinkData *>(ptr));

    case DB_NAMES_DATA:                 break;
    case DB_OTYPE_DATA:                 break;

    case DB_STRING_DATA:                return QString::fromStdString(*static_cast<const StringData *>(ptr));

    case DB_TEXT_DATA:                  break;
    case DB_TEXT_FILE_DATA:             break;
    case DB_TIME_DATA:                  break;
    case DB_TIME_INTERVAL_DATA:         break;
    }

    return QVariant();
}

static QVariant DataPtrToVariant(QSharedPointer<Data> ptr) {

    if(ptr.isNull()) {
        return QVariant();
    }
    return DataPtrToVariant(ptr.data());
}

/**
 * @brief DataTypeToString
 * DataType的类型转字符串
 */
static QString DataTypeToString(DataType type) {

    switch(type) {
    case BOOLEAN_DATA:              return "BOOLEAN_DATA";
    case SHORT_DATA:                return "SHORT_DATA";
    case INTEGER_DATA:              return "INTEGER_DATA";
    case LONGLONG_DATA:             return "LONGLONG_DATA";
    case FLOAT_DATA:                return "FLOAT_DATA";
    case STRING_DATA:               return "STRING_DATA";
    case LINK_DATA:                 return "LINK_DATA";
    case TIME_DATA:                 return "TIME_DATA";
    case CONTAINER_DATA:            return "CONTAINER_DATA";
    case MULTIPLE_CHOICE_DATA:      return "MULTIPLE_CHOICE_DATA";
    case FILE_DATA:                 return "FILE_DATA";
    case OBJECT_TYPE_DATA:          return "OBJECT_TYPE_DATA";
    case ATTRIBUTE_TYPE_DATA:       return "ATTRIBUTE_TYPE_DATA";
    case TIME_INTERVAL_DATA:        return "TIME_INTERVAL_DATA";
    case TEXT_FILE_DATA:            return "TEXT_FILE_DATA";
    case ATTRIBUTE_TYPE_LIST_DATA:  return "ATTRIBUTE_TYPE_LIST_DATA";
    case STATISTIC_DATA:            return "STATISTIC_DATA";
    case PEAK_LONG_DATA:            return "PEAK_LONG_DATA";
    case PEAK_FLOAT_DATA:           return "PEAK_FLOAT_DATA";
    }
    return "";
}

/**
 * @brief createDataPtr
 * 创建data的智能指针
 * @param type 如：BOOLEAN_DATA、LONGLONG_DATA等
 */
static QSharedPointer<Data> createDataPtr(DataType type, const QVariant &defaultValue = QVariant())
{
    switch(type) {

    case BOOLEAN_DATA:              //return "BOOLEAN_DATA";
    case SHORT_DATA:                //return "SHORT_DATA";
    case INTEGER_DATA:              return QSharedPointer<Data>(new IntegerData(defaultValue.toInt()));
    case LONGLONG_DATA:             return QSharedPointer<Data>(new LongLongData(defaultValue.toLongLong()));
    case FLOAT_DATA:                return QSharedPointer<Data>(new FloatData(defaultValue.toFloat()));
    case STRING_DATA:               return QSharedPointer<Data>(new StringData(defaultValue.toString().toStdString()));

    case LINK_DATA:                 return QSharedPointer<Data>(new LinkData(StringToObId(defaultValue.toString())));
    case TIME_DATA:                 break;//return "TIME_DATA";
    case CONTAINER_DATA:            break;//return "CONTAINER_DATA";
    case MULTIPLE_CHOICE_DATA:      return QSharedPointer<Data>(new IntegerData(defaultValue.toInt()));
    case FILE_DATA:                 //return "FILE_DATA";
    case OBJECT_TYPE_DATA:          //return "OBJECT_TYPE_DATA";
    case ATTRIBUTE_TYPE_DATA:       //return "ATTRIBUTE_TYPE_DATA";
    case TIME_INTERVAL_DATA:        //return "TIME_INTERVAL_DATA";
    case TEXT_FILE_DATA:            //return "TEXT_FILE_DATA";
    case ATTRIBUTE_TYPE_LIST_DATA:  //return "ATTRIBUTE_TYPE_LIST_DATA";
    case STATISTIC_DATA:            //return "STATISTIC_DATA";
    case PEAK_LONG_DATA:            //return "PEAK_LONG_DATA";
    case PEAK_FLOAT_DATA:           //return "PEAK_FLOAT_DATA";

    default:break;
    }
    //std::cout << ErrorDataType(DataTypeToString(type)).toLocal8Bit().data() << std::endl;
    return QSharedPointer<Data>();
}

OMSQuery::OMSQuery(OMSDatabase *database, QObject *parent)
    : QObject(parent)
    , m_database(database)
    , m_model(new QStandardItemModel(this))
    , m_model_row(-1)
    , m_model_clear_every_time(true)
{
}

//select obid from CCTVController where PointAddress = ? and Address = ?
bool OMSQuery::selectObIdList(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(m_lastQuery = sql, content)) {
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
        for(const SqlAnd &info : content.sql_and) {
            //create data
            AType atype = m_database->matchAType(info.field_name);
            DataType dtype = m_database->matchDataType(atype);
            QSharedPointer<Data> valuePtr = createDataPtr(dtype, info.value);
            if(valuePtr.isNull()) {
                m_lastError = ErrorDataType(DataTypeToString(dtype));
                return false;
            }
            //condition
            condVect.append(atype, stringToOperator(info.op), valuePtr);
        }
        //find
        OType otype = m_database->matchOType(content.table.toLocal8Bit().data());
        int count = m_database->find(otype, condVect.constData(), condVect.size());
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
        item->setData(LINK_DATA, RoleDataType);
        m_model->appendRow({item});
    }

    return true;
}

//select Name, PointAddress from OMS where obid = ?
bool OMSQuery::selectAttributeByObId(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(m_lastQuery = sql, content)) {
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
    for(int k = 0; k < content.sql_field_list.count(); k ++) {
        horizontalLabels << content.sql_field_list[k];
    }
    m_model->setHorizontalHeaderLabels(horizontalLabels);

    try {
        //fill model
        QList<QStandardItem *> itemList;
        for(int k = 0; k < content.sql_field_list.count(); k ++) {
            QByteArray field_name = content.sql_field_list[k];
            //create data
            AType atype = m_database->matchAType(field_name.data());
            DataType dtype = m_database->matchDataType(atype);
            QSharedPointer<Data> dataPtr = createDataPtr(dtype);
            if(dataPtr.isNull()) {
                m_lastError = ErrorDataType(DataTypeToString(dtype));
                return false;
            }
            //read
            Request req(StringToObId(obid), atype, dataPtr.data());
            m_database->read(&req, 1);

            itemList << new QStandardItem();
            itemList.last()->setData(DataPtrToVariant(dataPtr), Qt::DisplayRole);
            itemList.last()->setData(dtype, RoleDataType);
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
    if(!p.parse(m_lastQuery = sql, content, bindvalueList)) {
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


//select count from CCTVController where PointAddress = ? and Address = ?
bool OMSQuery::selectCount(const QString &sql)
{
    Parser p;
    SqlContent content;
    if(!p.parse(m_lastQuery = sql, content)) {
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
        for(const SqlAnd &info : content.sql_and) {
            //create data
            AType atype = m_database->matchAType(info.field_name);
            DataType dtype = m_database->matchDataType(atype);
            QSharedPointer<Data> valuePtr = createDataPtr(dtype, info.value);
            if(valuePtr.isNull()) {
                m_lastError = ErrorDataType(DataTypeToString(dtype));
                return false;
            }
            //condition
            condVect.append(atype, stringToOperator(info.op), valuePtr);
        }
        //find
        count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());

        //fill model
        QStandardItem *item = new QStandardItem;
        item->setData(count, Qt::DisplayRole);
        item->setData(INTEGER_DATA, RoleDataType);
        m_model->appendRow({item});

    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    return true;
}

//select obid, Name, PointAddress from CCTVController where PointAddress = ? and Address = ?
bool OMSQuery::selectAttribute(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(m_lastQuery = sql, content)) {
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
        for(const SqlAnd &info : content.sql_and) {
            //create data
            AType atype = m_database->matchAType(info.field_name);
            DataType dtype = m_database->matchDataType(atype);
            QSharedPointer<Data> valuePtr = createDataPtr(dtype, info.value);
            if(valuePtr.isNull()) {
                m_lastError = ErrorDataType(DataTypeToString(dtype));
                return false;
            }
            //condition
            condVect.append(atype, stringToOperator(info.op), valuePtr);
        }

        //find
        int count = m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size());
        obidVect.resize(count);
        m_database->find(m_database->matchOType(content.table.toLocal8Bit().data()), condVect.constData(), condVect.size(), obidVect.data(), obidVect.size());
    } catch(Exception &e) {
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }

    //horizontalLables: obid, Name, PointAddress
    QStringList labels;
    for(const QString &field_name : content.sql_field_list)
        labels << field_name;
    m_model->setHorizontalHeaderLabels(labels);

    try {
        //file model
        for(const ObId &obid : obidVect) {
            //select field from oms where obid = ?
            QList<QStandardItem *> itemList;
            if(labels.contains(ObidName)) {
                itemList << new QStandardItem;
                itemList.last()->setData(obid, Qt::DisplayRole);
                itemList.last()->setData(LINK_DATA, RoleDataType);
            }

            for(int k = 0; k < content.sql_field_list.count(); k ++) {
                QByteArray field_name = content.sql_field_list[k];
                if(field_name == ObidName)
                    continue;

                //create data
                AType atype = m_database->matchAType(field_name);
                DataType dtype = m_database->matchDataType(atype);
                QSharedPointer<Data> valuePtr = createDataPtr(dtype);
                if(valuePtr.isNull()) {
                    m_lastError = ErrorDataType(DataTypeToString(dtype));
                    return false;
                }

                //read
                Request req(obid, atype, valuePtr.data());
                m_database->read(&req, 1);

                itemList << new QStandardItem();
                itemList.last()->setData(DataPtrToVariant(valuePtr), Qt::DisplayRole);
                itemList.last()->setData(dtype, RoleDataType);
            }//~for
            m_model->appendRow(itemList);
        }//~for
    } catch (Exception &e){
        m_lastError = QString::fromStdString(e.getDescription());
        return false;
    }
    return true;
}

//write into OMS (obid, Name) values(?, ?)
bool OMSQuery::writeAtrribute(const QString &sql)
{
    Parser p;
    SqlContent content;

    if(!p.parse(m_lastQuery = sql, content)) {
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
            //create data
            AType atype = m_database->matchAType(bind.field_name);
            DataType dtype = m_database->matchDataType(atype);
            QSharedPointer<Data> fieldData = createDataPtr(dtype, bind.value);
            if(fieldData.isNull()) {
                m_lastError = ErrorDataType(bind.field_name);
                return false;
            }

            AType nAType = m_database->matchAType(bind.field_name);
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

QVariant OMSQuery::value(int index, int role)
{
    QStandardItem *item = m_model->item(m_model_row, index);
    return item? item->data(role) : QVariant();
}

QVariant OMSQuery::value(const QString &field_name, int role)
{
    for(int k = 0; k < m_model->columnCount(); k ++) {
        if(m_model->headerData(k, Qt::Horizontal).toString() == field_name) {
            return value(k, role);
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
        query.exec("select obid from CCTVController where PointAddress = ? and Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("selectObIdList");
        query.exec("select obid from CCTVController where PointAddress = 10 and Address = 28");
        query.printModel("selectObIdList");

        query.exec("select count from CCTVController where PointAddress = ? and Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("selectCount");
        query.exec("select count from CCTVController where PointAddress = 10 and Address = 28");
        query.printModel("selectCount");

        query.exec("select Name, PointAddress from OMS where obid = ?",
                   QVariantList() << "2023055425537  ");
        query.printModel("selectAttributeByObId");
        query.exec("select Name, PointAddress from OMS where obid = 2023055425537");
        query.printModel("selectAttributeByObId");

        query.exec("select obid, Name, PointAddress from CCTVController where PointAddress = ? and Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("SelectAttribute");
        query.exec("select obid, Name, PointAddress from CCTVController where PointAddress = 10 and Address = 28");
        query.printModel("SelectAttribute");

        query.exec("select Name, PointAddress from CCTVController where PointAddress = ? and Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("SelectAttribute");
        query.exec("select Name, PointAddress from CCTVController where PointAddress = 10 and Address = 28");
        query.printModel("SelectAttribute");

        query.exec("select obid from CCTVController where PointAddress = ? and Address = ?",
                   QVariantList() << 10 << 28);
        query.printModel("SelectAttribute");
        query.exec("select obid from CCTVController where PointAddress = 10 and Address = 28");
        query.printModel("SelectAttribute");


        //test for write into
        //  select obid, Name
        query.exec("select obid, Name, PointAddress from CCTVController where PointAddress = ? and Address = ?",
                   QVariantList() << 10 << 28);
        query.next();
        ObId obid = StringToObId(query.value("obid").toString());
        QString Name = query.value("Name").toString();
        //  482 "2023055425537" "马家园车辆段"
        //  chang Name
        Name += "(1)";
        //  write
        query.exec("write into OMS (obid, Name) values(?, ?)",
                   QVariantList() << obid << Name);
        //  read
        query.exec("select obid, Name, PointAddress from CCTVController where PointAddress = ? and Address = ?",
                   QVariantList() << 10 << 28);
        query.next();
        //  check
        if(Name != query.value("Name").toString()) {
            qDebug() << "Error: test failed";
        } else {
            Name = query.value("Name").toString();
            Name.chop(3);
            //restore
            query.exec("write into OMS (obid, Name) values(?, ?)",
                       QVariantList() << obid << Name);
            query.exec("select obid, Name, PointAddress from CCTVController where PointAddress = ? and Address = ?",
                       QVariantList() << 10 << 28);
            query.next();
            qDebug() << __LINE__ << query.value("obid").toString() << query.value("Name").toString();
        }

    }

    //serialExec
    {
        OMSQuery query(m_database);

        query.exec("select obid, Name from ObjectType where Name = Camera");
        query.serialExec("select ObjectSpecifier,Name from ObjectAttribute "
                         "where Name = FaultState and ParentLink=?");
        query.serialExec("select obid, Name, Rank from Choice where ParentLink=?");
        query.printModel("serialExec");


        query.exec("select obid, Name from ObjectType where Name = ?", QVariantList() << "Camera");
        query.serialExec("select ObjectSpecifier,Name from ObjectAttribute "
                         "where Name = FaultState and ParentLink=?");
        query.serialExec("select obid, Name, Rank from Choice where ParentLink=?");
        query.printModel("serialExec");
    }
    qDebug() << "test end";
}

QMap<int, QString> OMSQuery::getChoiceData(const QString &table, const QString &attr)
{
    OMSQuery &query = *this;

    QMap<int ,QString> map;
    query.exec("select obid, Name from ObjectType where Name = " + table);
    query.serialExec("select ObjectSpecifier,Name from ObjectAttribute "
                     "where Name = " + attr + " and ParentLink=?");
    query.serialExec("select obid, Name, Rank from Choice where ParentLink=?");
    while(query.next()) {
        map.insert(query.value("Rank").toInt(), query.value("Name").toString());
    }
    return map;
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

void OMSQuery::ConditionVector::append(AType aType, Comparison comparison, QSharedPointer<Data> valuePtr)
{
    m_ptrVector << valuePtr;
    m_conditionVector.append(Condition(aType, comparison, valuePtr.data()));
}

void OMSQuery::ConditionVector::print()
{
    qDebug() << "ConditionVector";
    qDebug() << "size:" << this->size();
    for(Condition d : m_conditionVector) {

        qDebug() << "type:" << d.getAType();
        qDebug() << "comparison:" << d.getComparison();
        qDebug() << "data:" << DataPtrToVariant(d.getData());
        qDebug() << "";
    }
}
