#include "mainview/tracks/interpolatinggraph.h"

#include "common/plotsettings.h"
#include "datamodel/doubleseries.h"
#include "mainview/tracks/qcustomplot.h"

InterpolatingGraph::InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d, PlotSettings::ScalingMode scalingMode, PlotSettings::ScaleType scaleType) :
    Graph(plot),
    series(d),
    plot(plot),
    lastUpdate(-1),
    currentMin(std::numeric_limits<double>::max()),
    currentMax(std::numeric_limits<double>::min()),
    currentScalingMode(PlotSettings::NOSCALING),
    currentScaleType(PlotSettings::LINSCALE)
{
    setObjectName("InterpolatingGraph");
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
    connect(&series, SIGNAL(offsetChanged()), this, SLOT(onOffsetChanged()));

    graph = plot->addGraph();
    // Ensure value axis is visible.
    plot->yAxis->setVisible(true);
    plot->yAxis->setGrid(true);

    configureAppearance(graph);
    initialize(graph, series);
    rescale(scalingMode, scaleType);

    updatePlot(scalingMode);
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
    rescale(settings.scalingMode, settings.scaleType(series.fullName()));
    updatePlot(settings.scalingMode);
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
//        updateMetadata(i.value());
        graph->addData(i.key(), i.value());
    }
}

void InterpolatingGraph::rescale(PlotSettings::ScalingMode scalingMode, PlotSettings::ScaleType scaleType)
{
    switch (scalingMode) {
    case PlotSettings::NOSCALING:
        if (scalingMode != currentScalingMode) {
            // This means that the graph's data need to be reset.
            graph->clearData();
            initialize(graph, series);
        } else {
            // Nothing to be done here.
            // The graph's individual scale type has no significance in this case;
            // the plot's scale type defines the appearance of its graphs.
        }
        // Ensure value axis is visible.
        plot->yAxis->setVisible(true);
        plot->yAxis->setGrid(true);
        break;
    case PlotSettings::MINMAXSCALING:
        if (scalingMode != currentScalingMode || scaleType != currentScaleType) {
            // The graph's data need to be rescaled to use the plot's full height.
            // If only the scale type has changed, we need to rescale anyway.
            scaleToRange(0.0, 1.0, scaleType);
        } else {
            // Nothing to be done here, as neither scaling mode nor scale type have changed.
        }
        // Don't need the axis in this case, as it would have no informative value.
        break;
    }

    currentScalingMode = scalingMode;
    currentScaleType = scaleType;
}

void InterpolatingGraph::scaleToRange(double lower, double upper, PlotSettings::ScaleType scaleType)
{
    if (scaleType == PlotSettings::NOT_SCALABLE) {
        // nothing to be done here
        return;
    }

    if (lower > upper) {
        // not a valid range
        return;
    }

    graph->clearData();

    auto const& data = series.getData();
    if (data.keys().takeLast() != lastUpdate) {
        updateMetadata(series.getData(lastUpdate, data.keys().takeLast()));
    }

    // rescale to new range
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        double scaledValue = i.value();
        switch (scaleType) {
        case PlotSettings::LINSCALE:
            scaledValue = ((i.value() - currentMin) / (currentMax - currentMin)) * (upper - lower) + lower;
            break;
        case PlotSettings::LOGSCALE: {
            double log10 = qLn(10);
            double rangeFac = 1e-3;

            if (std::abs(currentMin) > currentMax) {
                // negative interval is wider

                // The log scale boundaries may never be zero or carry the wrong sign.
                // Cut off above rangeFac or currentMax, whichever is closer to zero (while still negative).
                double logScaleMin = std::min(-rangeFac, currentMin);
                double logScaleMax = std::min(-rangeFac, currentMax);

                if (i.value() > logScaleMax) {
                    // invalid value. Draw outside visible area.
                    scaledValue = upper + (upper - lower)*0.75;
                } else {
                    scaledValue = upper - ((qLn(i.value()/logScaleMax)/log10) / (qLn(logScaleMin/logScaleMax)/log10)) * (upper - lower);
                }
            } else {
                // positive interval is wider

                // The log scale boundaries may never be zero or carry the wrong sign.
                // Cut off below rangeFac or currentMin, whichever is closer to zero (while still positive).
                double logScaleMin = std::max(rangeFac, currentMin);
                double logScaleMax = std::max(rangeFac, currentMax);

                if (i.value() < logScaleMin) {
                    // invalid value. Draw outside visible area.
                    scaledValue = lower - (upper - lower)*0.75;
                } else {
                    scaledValue = lower + ((qLn(i.value()/logScaleMin)/log10) / (qLn(logScaleMax/logScaleMin)/log10)) * (upper - lower);
                }
            }
            break;
        }
        default:
            // unknown or unscalable scale type
            break;
        }
        graph->addData(i.key(), scaledValue);
    }
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{
    auto const& data = series.getData(lastUpdate, timestamp);

    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        updateMetadata(i.value());
        switch (currentScalingMode) {
        case PlotSettings::NOSCALING:
            graph->addData(i.key(), i.value());
            break;
        case PlotSettings::MINMAXSCALING:
            scaleToRange(0.0, 1.0, currentScaleType);
            break;
        }
    }

    lastUpdate = timestamp;
    updatePlot(currentScalingMode);
}

void InterpolatingGraph::onOffsetChanged()
{
    graph->clearData();
    initialize(graph, series);
    rescale(currentScalingMode, currentScaleType);
    updatePlot(currentScalingMode);
}

void InterpolatingGraph::updateMetadata(QMap<qint64, double> data)
{
    for (QMap<qint64, double>::const_iterator i = data.constBegin(); i != data.constEnd(); i++) {
        updateMetadata(i.value());
    }
}

void InterpolatingGraph::updateMetadata(double value)
{
    if (value > currentMax) {
        currentMax = value;
    }

    if (value < currentMin) {
        currentMin = value;
    }
}

void InterpolatingGraph::updatePlot(PlotSettings::ScalingMode scalingMode)
{
    switch (scalingMode) {
    case PlotSettings::NOSCALING:
        plot->rescaleValueAxes();
        break;
    default:
        // no rescaling
        break;
    }

    plot->replot();
}
