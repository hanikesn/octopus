#include "gui/interpolatinggraph.h"

#include "gui/qcustomplot.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d) :
    series(d),
    plot(plot)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));

    graph = plot->addGraph();

    configureAppearance();
    initialize();

    plot->replot();
}

InterpolatingGraph::~InterpolatingGraph()
{
    // graph is deleted by plot upon removal
    plot->removeGraph(graph);
    plot->replot();
}

QString InterpolatingGraph::dataSeriesName()
{
    return series.fullName();
}

void InterpolatingGraph::configureAppearance()
{
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCP::ssDisc);
    graph->setScatterSize(4);
}

void InterpolatingGraph::initialize()
{
    auto const& data = series.getData();
    auto i = data.constBegin();
    while (i != data.constEnd()) {
        graph->addData(i.key(), i.value());
        ++i;
    }
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{
    double data = series.getData(timestamp);
    graph->addData(timestamp, data);

    plot->replot();
}
