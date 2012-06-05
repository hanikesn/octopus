#include "gui/discretegraph.h"

DiscreteGraph::DiscreteGraph(QCustomPlot *plot, const StringSeries &s) :
    series(s)
{
    graph = plot->addGraph();
    configureAppearance();

    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
}

void DiscreteGraph::configureAppearance()
{
    graph->setLineStyle(QCPGraph::lsImpulse);
    graph->setScatterStyle(QCPGraph::ssTriangleInverted);
    graph->setScatterSize(4);
}

void DiscreteGraph::onNewData(qint64 timestamp)
{

}
