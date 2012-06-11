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
    graph->setScatterStyle(QCP::ssTriangleInverted);
    graph->setScatterSize(4);
}

void DiscreteGraph::initialize()
{
    QMap<qint64, QString>::const_iterator i = series.getData().constBegin();
    while (i != series.getData().constEnd()) {
        graph->addData(i.key(), IMPULSE_HEIGHT);
        // Create a tracer for each data point and use its position as anchor for the label.
        addLabel(i.value(), addTracer(i.key(), 0)->position);

        ++i;
    }
}

QCPItemTracer* DiscreteGraph::addTracer(double graphKey, double size)
{
    QCPItemTracer *tracer = new QCPItemTracer(plot);
    tracer->setGraph(graph);
    tracer->setGraphKey(graphKey);

    tracer->setStyle(QCPItemTracer::tsCircle);
    tracer->setSize(size);

    return tracer;
}

QCPItemText* DiscreteGraph::addLabel(const QString& text, QCPItemAnchor *parentAnchor)
{
    QCPItemText *label = new QCPItemText(plot);
    if (text != "") {
        label->setText(text);
    } else {
        label->setText("Unlabeled");
    }

    label->setRotation(315);
    label->setPositionAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    label->position->setParentAnchor(parentAnchor);
    // (0, 0) is exactly on top of the parent anchor (see documentation to QCPItemPosition).
    label->position->setCoords(0, -7);

    return label;
}

void DiscreteGraph::onNewData(qint64 timestamp)
{
    graph->addData(timestamp, IMPULSE_HEIGHT);
    // Create a tracer for the new data point and use its position as anchor for the label.
    addLabel(series.getData(timestamp), addTracer(timestamp, 0)->position);

    plot->replot();
}
