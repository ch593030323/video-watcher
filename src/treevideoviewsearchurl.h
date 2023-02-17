#ifndef TREEVIDEOVIEWSEARCHURL_H
#define TREEVIDEOVIEWSEARCHURL_H


#include "treevideoviewsearch.h"
/**
 * @brief The TreeVideoViewSearchUrl class
 * 支持多数据源搜索的树形控件
 */
class TreeVideoViewSearchUrl : public TreeVideoViewSearch
{
    Q_OBJECT
public:
    TreeVideoViewSearchUrl(QWidget *parent = 0);

protected:
    virtual void addToPlayThread(const QString &url);

};

#endif // TREEVIDEOVIEWSEARCHURL_H
