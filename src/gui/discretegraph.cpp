#include "gui/discretegraph.h"

#include "gui/qcustomplot.h"

const QCPRange RANGE(0.0, 1.0);
const double IMPULSE_HEIGHT = RANGE.lower + 0.4 * RANGE.size();

DiscreteGraph::DiscreteGraph(QCustomPlot *plot, const StringSeries &s) :
    series(s),
    plot(plot)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));

    graph = plot->addGraph(plot->xAxis, plot->yAxis2);
    plot->yAxis2->setRange(RANGE);
    plot->yAxis2->setVisible(false);
    configureAppearance();
    initialize();

    plot->replot();
}

DiscreteGraph::~DiscreteGraph()
{
    // graph is deleted by plot upon removal
    plot->removeGraph(graph);
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
        // TODO(Steffi) : Use data as labels for the graph
        graph->addData(i.key(), IMPULSE_HEIGHT);
        ++i;
    }
}

void DiscreteGraph::onNewData(qint64 timestamp)
{
    // TODO(Steffi) : Use data as labels for the graph
    QString data = series.getData(timestamp);
    graph->addData(timestamp, IMPULSE_HEIGHT);

    plot->replot();
}
