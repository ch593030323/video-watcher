#include "jsoncpppath.h"

#include <QFile>
#include <QtDebug>
#include <QQueue>

JsonCppPath::JsonCppPath(const QString &jsonpath, QObject *parent)
    : QObject(parent)
    , m_jsonpath(jsonpath)
{
}

JsonCppPath::~JsonCppPath()
{
}

bool JsonCppPath::setValue(const QString &path, const QVariant &var)
{
    if(Json::Value *jsonPtr = mkpath(path)) {
        *jsonPtr = varianttoJsonValue(var);
        return true;
    }
    m_errorString = "path is not existed.";
    return false;
}

bool JsonCppPath::setJsonValue(const QString &path, const Json::Value &var)
{
    if(Json::Value *jsonPtr = mkpath(path)) {
        *jsonPtr = var;
        return true;
    }
    m_errorString = "path is not existed.";
    return false;
}

bool JsonCppPath::parse()
{
    QByteArray content;
    QFile file(m_jsonpath);
    if(!file.open(QFile::ReadOnly)) {
        m_errorString = file.errorString();
        return false;
    }
    content = file.readAll();

    Json::Reader reader;
    if(!reader.parse(content.begin(), content.end(), m_json)) {
        m_errorString = QString::fromStdString(reader.getFormatedErrorMessages());
        return false;
    }

    return true;
}

QString JsonCppPath::errorString()
{
    return m_errorString;
}

QVariant JsonCppPath::value(const QString &path, const QVariant &defaultValue)
{
    if(Json::Value *jsonPtr = jsonValuePtr(path)) {
        return jsonValuetoVariant(*jsonPtr);
    }
    return defaultValue;
}

QVariant JsonCppPath::valueArg(const QString &path, int arg0, const QVariant &defaultValue)
{
    return value(path.arg(arg0), defaultValue);
}

Json::Value JsonCppPath::jsonValue(const QString &path, const Json::Value &defaultValue)
{
    if(Json::Value *jsonPtr = jsonValuePtr(path)) {
        return *jsonPtr;
    }
    return defaultValue;
}

uint JsonCppPath::size(const QString &path)
{
    if(Json::Value *jsonPtr = jsonValuePtr(path)) {
        return jsonPtr->size();
    }
    return 0;
}

bool JsonCppPath::remove(const QString &path)
{
    if(Json::Value *jsonPtr = jsonValuePtr(path)) {
        *jsonPtr = Json::Value(Json::nullValue);
        return true;
    }
    m_errorString = "path is not existed.";
    return false;
}

QByteArray JsonCppPath::toJson()
{
    return QByteArray::fromStdString(Json::StyledWriter().write(m_json));
}

bool JsonCppPath::saveToFile()
{
    QFile file(m_jsonpath);
    if(!file.open(QFile::WriteOnly)) {
        m_errorString = file.errorString();
        return false;
    }
    file.write(toJson());
    file.close();
    return true;
}

QString JsonCppPath::fileName()
{
    return m_jsonpath;
}

QVariant JsonCppPath::jsonValuetoVariant(Json::Value value)
{
    switch(value.type()) {
    case Json::nullValue:     return QVariant(); ///< 'null' value
    case Json::intValue:      return value.asInt();      ///< signed integer value
    case Json::uintValue:     return value.asUInt();     ///< unsigned integer value
    case Json::realValue:     return value.asDouble();     ///< double value
    case Json::stringValue:   return QString::fromStdString(value.asString());   ///< UTF-8 string value
    case Json::booleanValue:  return value.asBool();  ///< bool value
    case Json::arrayValue:    ///< array value (ordered list)
    {
        QVariantList list;
        for(uint k = 0; k < value.size(); k ++) {
            list << jsonValuetoVariant(value[k]);
        }
        return list;
    }
    case Json::objectValue:   ///< object value (collection of name/value pairs).
    {
        QVariantMap map;
        Json::Value::Members keys = value.getMemberNames();
        for(int k = 0; k < keys.size(); k ++) {
            map.insert(QString::fromStdString(keys.at(k)), jsonValuetoVariant(value[keys.at(k)]));
        }
        return map;
    }
    default:
        break;
    }
    return QVariant();
}

Json::Value JsonCppPath::varianttoJsonValue(const QVariant &value)
{
    switch(value.type()) {
    case QVariant::Invalid:  break;
    case QVariant::Int:      return Json::Value(value.toInt());
    case QVariant::UInt:     return Json::Value(value.toUInt());
    case QVariant::Double:   return Json::Value(value.toDouble());
    case QVariant::String:   return Json::Value(value.toString().toStdString());
    case QVariant::StringList:   {
        Json::Value arr(Json::arrayValue);
        for(const QString &var : value.toStringList()) {
            arr.append(var.toStdString());
        }
        return arr;
    }
    case QVariant::Bool:     return Json::Value(value.toBool());
    case QVariant::List: {
        Json::Value arr(Json::arrayValue);
        for(const QVariant &var : value.toList()) {
            arr.append(varianttoJsonValue(var));
        }
        return arr;
    }
    case QVariant::Map: {
        Json::Value obj(Json::objectValue);
        QVariantMap map = value.toMap();
        for(QVariantMap::iterator k = map.begin(); k != map.end(); k ++) {
            obj[k.key().toStdString()] = varianttoJsonValue(k.value());
        }
        return obj;
    }
    }
    return Json::Value();
}

Json::Value *JsonCppPath::jsonValuePtr(const QString &path)
{
    Json::Value *jsonPtr = &m_json;
    for(const QString &p : breakPath(path)) {
        if(p.startsWith("/")) {
            std::string key = p.mid(1).toStdString();
            if(jsonPtr->type() == Json::objectValue && jsonPtr->isMember(key)) {
                jsonPtr = &jsonPtr->operator[](key);
                continue;
            }
        }
        if(p.startsWith("[")) {
            uint key = p.mid(1).toUInt();
            if(jsonPtr->type() == Json::arrayValue && key < jsonPtr->size()) {
                jsonPtr = &jsonPtr->operator[](key);
                continue;
            }
        }
        return 0;
    }
    return jsonPtr;

}

Json::Value *JsonCppPath::mkpath(const QString &path)
{
    Json::Value *jsonPtr = &m_json;
    for(const QString &p : breakPath(path)) {
        if(p.startsWith("/")) {
            //类型不同时，移除旧的节点
            if(jsonPtr->type() != Json::objectValue)
                *jsonPtr = Json::Value(Json::nullValue);

            //新加节点
            jsonPtr = &jsonPtr->operator[](p.mid(1).toStdString());
        }
        if(p.startsWith("[")) {
            //类型不同时，移除旧的节点
            if(jsonPtr->type() != Json::arrayValue)
                *jsonPtr = Json::Value(Json::nullValue);

            //新加节点
            jsonPtr = &jsonPtr->operator[](p.mid(1).toUInt());
        }
    }
    return jsonPtr;
}

bool JsonCppPath::exists(const QString &path)
{
    return 0 != jsonValuePtr(path);
}

QStringList JsonCppPath::breakPath(const QString &path)
{
    //start width / or [
    if(path.indexOf(QRegExp("(?:/|\\[){1}")) != 0) {
        m_errorString = "Error path format.";
        return QStringList();
    }

    QStringList list;
    int from = 0;
    while((from = path.indexOf(QRegExp("(?:/|\\[)"), from) + 1) > 0) {
        int to = path.indexOf(QRegExp("(?:/|\\[)"), from);
        list << path.mid(from - 1, to - from + 1);
    }
    return list;
}
