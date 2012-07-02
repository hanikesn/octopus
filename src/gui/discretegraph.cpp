#include "gui/discretegraph.h"
#include "stringseries.h"

#include "gui/qcustomplot.h"

#include <algorithm>

const QCPRange RANGE(0.0, 1.0);
const double IMPULSE_HEIGHT = RANGE.lower + 0.4 * RANGE.size();

DiscreteGraph::DiscreteGraph(QCustomPlot *plot, const StringSeries &s) :
    Graph(plot),
    series(s),
    plot(plot),
    lastUpdate(-1)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
    connect(&series, SIGNAL(offsetChanged()), this, SLOT(onOffsetChanged()));

    graph = plot->addGraph(plot->xAxis, plot->yAxis2);
    plot->yAxis2->setRange(RANGE);
    plot->yAxis2->setVisible(false);

    configureAppearance(graph);
    initialize(graph, series);

    plot->replot();
}

QCPGraph* DiscreteGraph::getGraph()
{
    return graph;
}

QString DiscreteGraph::dataSeriesName()
{
    return series.fullName();
}

void DiscreteGraph::update(const PlotSettings &/*settings*/)
{
}

void DiscreteGraph::configureAppearance(QCPGraph *graph)
{
    graph->setLineStyle(QCPGraph::lsImpulse);
    graph->setScatterStyle(QCP::ssTriangleInverted);
    graph->setScatterSize(4);
}

void DiscreteGraph::initialize(QCPGraph *graph, const StringSeries &series)
{
    graph->setName(series.fullName());

    auto const& data = series.getData();
    QMap<qint64, QString>::const_iterator i = data.constBegin();
    while (i != data.constEnd()) {
        addData(i.key(), i.value());
        ++i;
    }
}

void DiscreteGraph::addData(qint64 timestamp, const QString &str)
{
    graph->addData(timestamp, IMPULSE_HEIGHT);
    // Create a tracer for each data point and use its position as anchor for the label.
    addLabel(str, addTracer(timestamp, 0)->position);
}

QCPItemTracer* DiscreteGraph::addTracer(double graphKey, double size)
{
    QCPItemTracer *tracer = new QCPItemTracer(plot);
    tracer->setGraph(graph);
    tracer->setGraphKey(graphKey);

    tracer->setStyle(QCPItemTracer::tsCircle);
    tracer->setSize(size);

    // make sure the tracer gets deleted with its graph
    tracer->setParent(graph);

    return tracer;
}

QCPItemText* DiscreteGraph::addLabel(const QString& text, QCPItemAnchor *parentAnchor)
{
    QCPItemText *label = new QCPItemText(plot);
    if (!text.isEmpty()) {
        label->setText(text);
    } else {
        label->setText("Unlabeled");
    }

    label->setRotation(315);
    label->setPositionAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    label->position->setParentAnchor(parentAnchor);
    // (0, 0) is exactly on top of the parent anchor (see documentation on QCPItemPosition).
    label->position->setCoords(0, -7);

    // make sure the label gets deleted with its graph
    label->setParent(graph);

    return label;
}

void DiscreteGraph::onNewData(qint64 timestamp)
{
    auto const& data = series.getData(lastUpdate, timestamp);

    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
	    addData(i.key(), i.value());
    }

    lastUpdate = timestamp;
    plot->replot();
}

void DiscreteGraph::onOffsetChanged()
{
    graph->clearData();
    initialize(graph, series);
    plot->replot();
}
