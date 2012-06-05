#ifndef DISCRETEGRAPH_H
#define DISCRETEGRAPH_H

#include "stringseries.h"
#include "qcustomplot.h"
#include "gui/graph.h"

class DiscreteGraph : public Graph
{
    Q_OBJECT
public:
    explicit DiscreteGraph(QCustomPlot *plot, const StringSeries &s);

    QString dataSeriesName();

private:
    const StringSeries &series;
    QCPGraph *graph;

    void configureAppearance();
    void initialize();

protected slots:
    void onNewData(qint64 timestamp);
};

#endif // DISCRETEGRAPH_H
