#include "videolayout.h"
#include "json/json.h"
#include "videocell.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QtDebug>

QString LayoutCell::url() const
{
    return url(device_obid);
}

QString LayoutCell::url(QString device_obid)
{
    return lds::selectValue("select url from wx_device where obid = '%1'", device_obid).toString();
}

int LayoutCell::indexOf(QRect rect, const QList<LayoutCell> &list)
{
    for(int k = 0; k < list.count(); k ++) {
        if(list[k].rect() == rect)
            return k;
    }
    return -1;
}

QRect LayoutCell::rect() const
{
    return QRect(pos.x, pos.y, column_spans, row_spans);
}

QByteArray LayoutInfo::toJson()
{
    Json::Value root;
    Json::FastWriter writer;

    root["column_count"] = column_count;
    root["row_count"] = row_count;

    Json::Value json_cells;
    for(int k = 0; k < cells.count(); k ++) {
        Json::Value json_cell;
        json_cell["x"] = cells[k].x();
        json_cell["y"] = cells[k].y();
        json_cell["column_spans"] = cells[k].column_spans;
        json_cell["row_spans"] = cells[k].row_spans;
        json_cell["device_obid"] = cells[k].device_obid.toStdString();

        json_cells.append(json_cell);
    }
    root["cells"] = json_cells;

    std::string json_file = writer.write(root);

    return QString::fromStdString(json_file).toLocal8Bit();
}

LayoutInfo LayoutInfo::readFrom(const QByteArray &json)
{
    LayoutInfo info;
    Json::Value value;
    Json::Reader reader;
    if(!reader.parse(json.begin(), json.end(), value)) {
        qDebug() << "json formate is error";
    }
    info.column_count = value["column_count"].asInt();
    info.row_count = value["row_count"].asInt();

    Json::Value cells = value["cells"];
    for(int k = 0; k < cells.size(); k ++) {
        LayoutCell cell;
        cell.pos.x = cells[k]["x"].asInt();
        cell.pos.y = cells[k]["y"].asInt();
        cell.column_spans   = cells[k]["column_spans"].asInt();
        cell.row_spans      = cells[k]["row_spans"].asInt();
        cell.device_obid    = cells[k]["device_obid"].asCString();

        info.cells.append(cell);
    }
    return info;
}

void LayoutInfo::update(const QMap<LayoutPos, VideoCell *> &map)
{
    //将videoWidget的摄像头id保存到layoutinfo里
    for(QMap<LayoutPos, VideoCell *>::const_iterator k = map.begin(); k != map.end(); k ++) {
        VideoCell *w = k.value();
        QString obid = w->device_obid();
        if(w->isVisible() && obid != "") {
            QList<LayoutCell> &cells = this->cells;
            int index = LayoutCell::indexOf(w->rectX(), cells);
            if(index >= 0) {
                cells[index].device_obid = obid;
            } else {
                cells << w->getInfo();
            }
        }
    }
}
