#ifndef TREEVIDEOWIDGET_H
#define TREEVIDEOWIDGET_H


#include "treevideosearch.h"
/**
 * @brief The TreeVideoWidget class
 * 支持多数据源搜索的树形控件
 */
class TreeVideoWidget : public TreeVideoSearch
{
    Q_OBJECT
public:
    TreeVideoWidget(QWidget *parent = 0);

protected:
    virtual void addToPlayThread(const QString &obid, const QString &url);

};

#endif // TREEVIDEOWIDGET_H
