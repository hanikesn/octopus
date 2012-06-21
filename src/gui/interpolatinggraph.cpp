#include "gui/interpolatinggraph.h"
#include "doubleseries.h"

#include "plotsettings.h"
#include "gui/qcustomplot.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d) :
    Graph(plot),
    series(d),
    plot(plot),
    lastUpdate(-1),
    offset(0)/*,
    currentScalingMode(ONEFORALL)*/
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));

    graph = plot->addGraph();
    plot->yAxis->setVisible(true);

    configureAppearance(graph);
    initialize(graph, series, offset);

    plot->rescaleValueAxes();
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

void InterpolatingGraph::update(PlotSettings settings, bool respectScalingMode)
{
//    rescale(settings.scalingMode(series.fullName()), settings.scaleType(series.fullName()));
//    setOffset(settings.offset(series.fullName()));
}

void InterpolatingGraph::configureAppearance(QCPGraph *graph)
{
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCP::ssDisc);
    graph->setScatterSize(4);
}

void InterpolatingGraph::initialize(QCPGraph *graph, const DoubleSeries &series, int offsetMicroSecs)
{
    graph->setName(series.fullName());

    auto const& data = series.getData();
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        graph->addData(i.key() + offsetMicroSecs, i.value());
    }
}

//void InterpolatingGraph::rescale(ScaleType scaleType, ScalingMode scalingMode)
//{
//    if (scalingMode != currentScalingMode) {
//        switch (scalingMode) {
//        case ONEFORALL:
//            // this means that the graph's data have been rescaled --> reset
//            graph->clearData();
//            initialize(graph, series, offset);
//            break;
//        case MINMAXEACH:
//            // the graph's data need to be rescaled to use the plot's full height
//            break;
//        default:
//            // ignore
//            break;
//        }
//    }

//    switch (scaleType) {
//    case LIN:
//        break;
//    case LOG:
//        break;
//    }
//}

void InterpolatingGraph::setOffset(int microSecs)
{
    if (offset != microSecs) {
        // use the data from the graph instead of from the series, in case they have been rescaled.
        QCPDataMap *newDataMap = new QCPDataMap();
        QCPDataMapIterator i(*(graph->data()));
        while (i.hasNext()) {
            i.next();
            newDataMap->insert(i.key() + microSecs, i.value());
        }
        graph->setData(newDataMap);
        offset = microSecs;
    }
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{
    auto const& data = series.getData(lastUpdate, timestamp);

    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        // TODO(Steffi): ggf. skalieren!
        graph->addData(i.key() + offset, i.value());
    }

    lastUpdate = timestamp;
    plot->rescaleValueAxes();
    plot->replot();
}
