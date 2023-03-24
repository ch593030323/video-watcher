#include "videolayout.h"
#include "videocell.h"
#include "jsoncpppath.h"

#include <QtDebug>

LayoutCell::LayoutCell()
{
    pos.x = 0;
    pos.y = 0;
    row_spans = 1;
    column_spans = 1;
}

int LayoutCell::indexOf(QRect rect, const QList<LayoutCell> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        if(list[k].rect() == rect)
            return k;
    }
    return -1;
}

int LayoutCell::indexOf(LayoutPos pos, const QList<LayoutCell> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        if(list[k].pos.value() == pos.value())
            return k;
    }
    return -1;
}

QRect LayoutCell::rect() const
{
    return QRect(pos.x, pos.y, column_spans, row_spans);
}

QString LayoutCell::toString() const
{
    return QString("pos:%1, row_spans: %2, column_spans:%3, url:%4")
            .arg(pos.value())
            .arg(row_spans)
            .arg(column_spans)
            .arg(url)
            ;
}

bool LayoutCell::isNull() const
{
    return column_spans == 1
            && row_spans == 1
            && url.trimmed().isEmpty();
}

QByteArray LayoutInfo::toJson()
{
    //QByteArray::fromStdString is not supported in qt4
    return QString::fromStdString(Json::StyledWriter()
                                  .write(toJsonValue())).toLocal8Bit();
}

Json::Value LayoutInfo::toJsonValue()
{
    Json::Value root;

    root["column_count"] = column_count;
    root["row_count"] = row_count;

    Json::Value json_cells;
    for(int k = 0; k < cells.count(); k ++) {
        const LayoutCell &cell = cells[k];

        if(cell.isNull())
            continue;

        Json::Value json_cell;
        json_cell["x"]              = cell.x();
        json_cell["y"]              = cell.y();
        json_cell["column_spans"]   = cell.column_spans;
        json_cell["row_spans"]      = cell.row_spans;
        json_cell["url"]            = cell.url.toStdString();

        json_cells.append(json_cell);
    }
    root["cells"] = json_cells;

    return root;
}

LayoutInfo LayoutInfo::readFrom(const QByteArray &json)
{
    Json::Value value;
    Json::Reader reader;
    if(!reader.parse(json.begin(), json.end(), value)) {
        qDebug() << "json formate is error";
    }
    return readFrom(value);
}

LayoutInfo LayoutInfo::readFrom(const Json::Value &value)
{
    JsonCppReader reader(value);

    LayoutInfo info;
    info.column_count = reader.value("/column_count").toInt();
    info.row_count = reader.value("/row_count").toInt();

    info.column_count = qMin(9, info.column_count);
    info.row_count = qMin(9, info.row_count);

    for(uint k = 0, count = reader.size("/cells"); k < count; k ++) {
        LayoutCell cell;
        cell.pos.x          = reader.valueArg("/cells[%1/x", k).toInt();
        cell.pos.y          = reader.valueArg("/cells[%1/y", k).toInt();
        cell.column_spans   = reader.valueArg("/cells[%1/column_spans", k).toInt();
        cell.row_spans      = reader.valueArg("/cells[%1/row_spans", k).toInt();
        cell.url            = reader.valueArg("/cells[%1/url", k).toString();

        info.cells.append(cell);
    }
    return info;
}

void LayoutInfo::update(const QMap<LayoutPos, VideoCell *> &map)
{
    //将videoWidget的摄像头id保存到layoutinfo里
    for(QMap<LayoutPos, VideoCell *>::const_iterator k = map.begin(); k != map.end(); k ++) {
        VideoCell *w = k.value();
        const LayoutCell &other = w->getInfo();
        qDebug() << __FUNCTION__ << __LINE__ << other.pos.value() << other.url << w->isVisible();
        if(w->isVisible()) {
            qDebug() << __FUNCTION__ << __LINE__ << other.pos.value() << other.url;
            QList<LayoutCell> &cells = this->cells;
            int index = LayoutCell::indexOf(w->getInfo().pos, cells);
            if(index >= 0) {
                cells[index].url = other.url;
            } else if(!w->getInfo().isNull()){
                cells << w->getInfo();
            }
        }
    }
}
