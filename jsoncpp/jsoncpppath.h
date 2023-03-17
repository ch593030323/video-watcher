#ifndef JSONCPPPATH_H
#define JSONCPPPATH_H

#include "json/json.h"

#include <QObject>
#include <QVariant>

class JsonCppPath : public QObject
{
    Q_OBJECT
public:
    JsonCppPath(const QString &jsonpath, QObject *parent = 0);
    ~JsonCppPath();

    /**
     * @brief setValue
     * json的赋值
	 * 支持：uint、int、bool、double、QString、QStringList、QVarintMap
     */
    bool setValue(const QString &path, const QVariant &var);
    bool setJsonValue(const QString &path, const Json::Value &var);

    /**
     * @brief value
     * 返回path对应的值，eg: value("/global/node/galaxy/earth/name", "apple");
	 * 支持：uint、int、bool、double、QString、QVariantList、QVarintMap
     */
    QVariant value(const QString &path, const QVariant &defaultValue = QVariant());
    /**
     * @brief valueArg
     * 等价于 value(path.arg(arg0), defaultValue);
     */
    QVariant valueArg(const QString &path, int arg0, const QVariant &defaultValue = QVariant());

    Json::Value jsonValue(const QString &path, const Json::Value &defaultValue = Json::Value(Json::nullValue));

    /**
     * @brief size
     * 返回array的size或object的size
     */
    uint size(const QString &path);

    bool remove(const QString &path);

public:
    QByteArray toJson();
    bool saveToFile();

    QString fileName();

    bool parse();
    bool parse(const QByteArray &content);
    QString errorString();

    static QVariant jsonValuetoVariant(Json::Value value);
    static Json::Value varianttoJsonValue(const QVariant &value);

    Json::Value *jsonValuePtr(const QString &path);

    Json::Value *mkpath(const QString &path);

    bool exists(const QString &path);

    /**
     * @brief breakPath
     * 转换: "/11/22/22" 到 (/11,/22,/33)
     */
    QStringList breakPath(const QString &path);

public:
    Json::Value     m_json;
    QString         m_errorString;
    QString         m_jsonpath;
    bool            m_hasSetValue;//标志：记录是否json被修改过
};

class JsonCppSettings : public JsonCppPath{
    Q_OBJECT
public:
    JsonCppSettings(const QString &jsonpath, QObject *parent = 0);
    virtual ~JsonCppSettings();
};


class JsonCppReader : public JsonCppPath{
    Q_OBJECT
public:
    JsonCppReader(const Json::Value &json, QObject *parent = 0);
    virtual ~JsonCppReader();
};


#endif // JSONCPPPATH_H
