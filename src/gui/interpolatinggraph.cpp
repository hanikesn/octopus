#include "gui/interpolatinggraph.h"
#include "doubleseries.h"

#include "plotsettings.h"
#include "gui/qcustomplot.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d) :
    Graph(plot),
    series(d),
    plot(plot),
    lastUpdate(-1),
    currentScalingMode(PlotSettings::NOSCALING),
    currentScaleType(d.defaultScaleType)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));

    graph = plot->addGraph();
    plot->yAxis->setVisible(true);

    configureAppearance(graph);
    initialize(graph, series);

    plot->rescaleValueAxes();
    plot->replot();
}

QCPGraph* InterpolatingGraph::getGraph()
{
    return graph;
}

PlotSettings::ScaleType InterpolatingGraph::getScaleType() const
{
    return currentScaleType;
}

QString InterpolatingGraph::dataSeriesName()
{
    return series.fullName();
}

void InterpolatingGraph::update(const PlotSettings &settings)
{
//    rescale(settings.scalingMode(series.fullName()), settings.scaleType(series.fullName()));
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
}

//void InterpolatingGraph::rescale(PlotSettings::ScaleType scaleType, PlotSettings::ScalingMode scalingMode)
//{
//    if (scalingMode != currentScalingMode) {
//        switch (scalingMode) {
//        case PlotSettings::NOSCALING:
//            // this means that the graph's data have been rescaled --> reset
//            graph->clearData();
//            initialize(graph, series);
//            break;
//        case PlotSettings::MINMAXSCALING:
//            // the graph's data need to be rescaled to use the plot's full height
//            break;
//        default:
//            // ignore
//            break;
//        }
//    }

//    switch (scaleType) {
//    case PlotSettings::LINSCALE:
//        break;
//    case PlotSettings::LOGSCALE:
//        break;
//    }
//}

void InterpolatingGraph::onNewData(qint64 timestamp)
{
    auto const& data = series.getData(lastUpdate, timestamp);

    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        // TODO(Steffi): ggf. skalieren!
        graph->addData(i.key(), i.value());
    }

    lastUpdate = timestamp;
    plot->rescaleValueAxes();
    plot->replot();
}
