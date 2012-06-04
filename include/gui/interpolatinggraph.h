#ifndef INTERPOLATINGGRAPH_H
#define INTERPOLATINGGRAPH_H

#include "doubleseries.h"
#include "qcustomplot.h"
#include "gui/graph.h"

class InterpolatingGraph : public Graph
{
    Q_OBJECT
public:
    explicit InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d);

private:
    const DoubleSeries &series;
    QCPGraph *graph;

protected slots:
    void onNewData(qint64 timestamp);
};

#endif // INTERPOLATINGGRAPH_H
