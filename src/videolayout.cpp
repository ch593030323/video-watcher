#include "videolayout.h"
#include "videocell.h"

#include <QtDebug>

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
            .arg(url);
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
        json_cell["x"] = cell.x();
        json_cell["y"] = cell.y();
        json_cell["column_spans"] = cell.column_spans;
        json_cell["row_spans"] = cell.row_spans;
        json_cell["url"] = cell.url.toStdString();

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
    LayoutInfo info;
    info.column_count = value["column_count"].asInt();
    info.row_count = value["row_count"].asInt();

    info.column_count = qMin(9, info.column_count);
    info.row_count = qMin(9, info.row_count);

    Json::Value cells = value["cells"];
    for(int k = 0; k < cells.size(); k ++) {
        LayoutCell cell;
        cell.pos.x = cells[k]["x"].asInt();
        cell.pos.y = cells[k]["y"].asInt();
        cell.column_spans   = cells[k]["column_spans"].asInt();
        cell.row_spans      = cells[k]["row_spans"].asInt();
        cell.url            = cells[k]["url"].asCString();

        info.cells.append(cell);
    }
    return info;
}

void LayoutInfo::update(const QMap<LayoutPos, VideoCell *> &map)
{
    //将videoWidget的摄像头id保存到layoutinfo里
    for(QMap<LayoutPos, VideoCell *>::const_iterator k = map.begin(); k != map.end(); k ++) {
        VideoCell *w = k.value();
        QString url = w->getInfo().url;
        if(w->isVisible()) {
            QList<LayoutCell> &cells = this->cells;
            int index = LayoutCell::indexOf(w->getInfo().pos, cells);
            if(index >= 0) {
                cells[index].url = url;
            } else if(!w->getInfo().isNull()){
                cells << w->getInfo();
            }
        }
    }
}
