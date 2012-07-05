/*
  An interpolating graph shows its data as dots connected by a line.
*/

#ifndef INTERPOLATINGGRAPH_H
#define INTERPOLATINGGRAPH_H

#include "plotsettings.h"
#include "gui/graph.h"

class DoubleSeries;
class QCPGraph;
class QCustomPlot;

class InterpolatingGraph : public Graph
{
    Q_OBJECT
public:
    explicit InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d, PlotSettings::ScalingMode scalingMode, PlotSettings::ScaleType scaleType);

    QCPGraph* getGraph();
    PlotSettings::ScaleType getScaleType() const;

    QString dataSeriesName();
    void update(const PlotSettings &settings);    

protected slots:
    void onNewData(qint64 timestamp);
    void onOffsetChanged();

private:
    const DoubleSeries &series;
    QCustomPlot *plot;
    QCPGraph *graph;

    /**
     * The time stamp (in µs) of the graph's last update.
     */
    qint64 lastUpdate;

    /**
     * The graph's current minimum value.
     */
    double currentMin;

    /**
     * The graph's current maximum value.
     */
    double currentMax;

    PlotSettings::ScalingMode currentScalingMode;
    PlotSettings::ScaleType currentScaleType;

    /**
     * Configures the appearance of the graph on the plot.
     */
    void configureAppearance(QCPGraph *graph);

    /**
     * Initializes the graph with the data from its data series.
     *      This function does not respect any scaling settings.
     */
    void initialize(QCPGraph *graph, const DoubleSeries &series);

    /**
     * Rescales the graph's data according to the given settings.
     */
    void rescale(PlotSettings::ScalingMode scalingMode, PlotSettings::ScaleType scaleType);

    /**
     * Rescales the data to the given range as if the range were scaled according to the given type.
     *      If the given range is not valid (i.e. lower > upper), no scaling is done.
     * @param lower The lower end of the range.
     * @param upper The upper end of the range.
     * @param scaleType The scale type to be mimicked.
     */
    void scaleToRange(double lower, double upper, PlotSettings::ScaleType scaleType);

    /**
     * Resets the graph's meta data to the values they were initialized to upon construction of this graph.
     */
    void resetMetadata();

    /**
     * Convenience function.
     */
    void updateMetadata(QMap<qint64, double> data);

    /**
     * Updates the meta information on the graph, such as the current minimum and maximum value.
     *      For resetting the meta data before the update, use /ref resetMetadata.
     */
    void updateMetadata(double value);

    /**
     * Updates the plot according to the given scaling mode.
     */
    void updatePlot(PlotSettings::ScalingMode scalingMode);
};

#endif // INTERPOLATINGGRAPH_H
