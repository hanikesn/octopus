#include "gui/discretegraph.h"

DiscreteGraph::DiscreteGraph(QCustomPlot *plot, const StringSeries &s) :
    series(s)
{
    graph = plot->addGraph();

    // TODO(Steffi): configure graph

    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
}

void DiscreteGraph::onNewData(qint64 timestamp)
{

}
