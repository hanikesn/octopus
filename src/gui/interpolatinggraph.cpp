#include "gui/interpolatinggraph.h"

#include "gui/qcustomplot.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d) :
    Graph(plot),
    series(d),
    plot(plot),
    lastUpdate(-1)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));

    graph = plot->addGraph();
    plot->yAxis->setVisible(true);

    configureAppearance(graph);
    initialize(graph, series);

    plot->replot();
}

QCPGraph* InterpolatingGraph::getGraph()
{
    return graph;
}

QString InterpolatingGraph::dataSeriesName()
{
    return series.fullName();
}

void InterpolatingGraph::configureAppearance(QCPGraph *graph)
{
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCP::ssDisc);
    graph->setScatterSize(4);
}

void InterpolatingGraph::initialize(QCPGraph *graph, const DoubleSeries &series)
{
    graph->setName(series.fullName());

    auto const& data = series.getData();
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
	    graph->addData(i.key(), i.value());
    }

    // TODO(Steffi): Rescale muss auch die anderen Graphen in der Spur berücksichtigen
    graph->rescaleValueAxis(true);
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{
    auto const& data = series.getData(lastUpdate, timestamp);

    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
	    graph->addData(i.key(), i.value());
    }

    lastUpdate = timestamp;
    plot->replot();
}
