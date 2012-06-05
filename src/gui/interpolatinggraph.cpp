#include "gui/interpolatinggraph.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d) :
    series(d)
{
    graph = plot->addGraph();
    configureAppearance();

    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
}

void InterpolatingGraph::configureAppearance()
{
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPGraph::ssDisc);
    graph->setScatterSize(4);
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{
    QList<double> data = series.getData(timestamp, timestamp);
    foreach (double d, data) {
        graph->addData(timestamp, d);
    }
}
