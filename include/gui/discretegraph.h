#ifndef DISCRETEGRAPH_H
#define DISCRETEGRAPH_H

#include "stringseries.h"
#include "gui/graph.h"

class QCustomPlot;
class QCPGraph;

class DiscreteGraph : public Graph
{
    Q_OBJECT
public:
    explicit DiscreteGraph(QCustomPlot *plot, const StringSeries &s);
    ~DiscreteGraph();

    QString dataSeriesName();

private:
    const StringSeries &series;
    QCustomPlot *plot;
    QCPGraph *graph;

    void configureAppearance();
    void initialize();

protected slots:
    void onNewData(qint64 timestamp);
};

#endif // DISCRETEGRAPH_H
