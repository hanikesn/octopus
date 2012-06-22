#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QtCore/QIntegerForSize>

class PlotSettings;
class QCPGraph;

class Graph : public QObject
{
    Q_OBJECT
public:
    Graph(QObject* parent) : QObject(parent) {}
    virtual ~Graph() {}

    virtual QCPGraph* getGraph() = 0;

    virtual QString dataSeriesName() = 0;
    virtual void update(const PlotSettings &settings) = 0;

protected slots:
    virtual void onNewData(qint64 timestamp) = 0;
};

#endif // GRAPH_H
