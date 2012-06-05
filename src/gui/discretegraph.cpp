#include "gui/discretegraph.h"

DiscreteGraph::DiscreteGraph(QCustomPlot *plot, const StringSeries &s) :
    series(s)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));

    graph = plot->addGraph();
    configureAppearance();
    initialize();

    plot->replot();
}

QString DiscreteGraph::dataSeriesName()
{
    return series.fullName();
}

void DiscreteGraph::configureAppearance()
{
    graph->setLineStyle(QCPGraph::lsImpulse);
    graph->setScatterStyle(QCPGraph::ssTriangleInverted);
    graph->setScatterSize(4);
}

void DiscreteGraph::initialize()
{
    QMap<qint64, QString>::const_iterator i = series.getData().constBegin();
    while (i != series.getData().constEnd()) {
        graph->addData(i.key(), 4.0);
        ++i;
    }
}

void DiscreteGraph::onNewData(qint64 timestamp)
{
    // TODO(Steffi) : Use data as labels for the graph
    QString data = series.getData(timestamp);
    graph->addData(timestamp, 4.0);

    emit needsReplot();
}
