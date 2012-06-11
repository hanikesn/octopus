#ifndef INTERPOLATINGGRAPH_H
#define INTERPOLATINGGRAPH_H

#include "doubleseries.h"
#include "gui/graph.h"

class QCustomPlot;
class QCPGraph;

class InterpolatingGraph : public Graph
{
    Q_OBJECT
public:
    explicit InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d);
    ~InterpolatingGraph();

    QString dataSeriesName();

private:
    const DoubleSeries &series;
    QCustomPlot *plot;
    QCPGraph *graph;

    void configureAppearance();
    void initialize();

protected slots:
    void onNewData(qint64 timestamp);
};

#endif // INTERPOLATINGGRAPH_H
