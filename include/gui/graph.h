#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QtCore/QIntegerForSize>

class Graph : public QObject
{
    Q_OBJECT
public:
    virtual ~Graph() {}
    virtual QString dataSeriesName() = 0;

signals:
    void needsReplot();

protected slots:
    virtual void onNewData(qint64 timestamp) = 0;
};

#endif // GRAPH_H
