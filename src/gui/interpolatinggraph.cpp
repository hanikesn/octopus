#include "gui/interpolatinggraph.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d) :
    series(d)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
    connect(this, SIGNAL(needsReplot()), plot, SLOT(replot()));

    graph = plot->addGraph();
    configureAppearance();
    initialize();

    plot->replot();
}

QString InterpolatingGraph::dataSeriesName()
{
    return series.fullName();
}

void InterpolatingGraph::configureAppearance()
{
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPGraph::ssDisc);
    graph->setScatterSize(4);
}

void InterpolatingGraph::initialize()
{
    QMap<qint64, double>::const_iterator i = series.getData().constBegin();
    while (i != series.getData().constEnd()) {
        graph->addData(i.key(), i.value());
        ++i;
    }
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{
    double data = series.getData(timestamp);
    graph->addData(timestamp, data);

    emit needsReplot();
}
