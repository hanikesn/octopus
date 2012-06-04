#include "gui/interpolatinggraph.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d) :
    series(d)
{
    graph = plot->addGraph();

    // TODO(Steffi): configure graph

    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{

}
