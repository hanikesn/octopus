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
    explicit InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d);

    QCPGraph* getGraph();
    PlotSettings::ScaleType getScaleType() const;
    void setScaleType(PlotSettings::ScaleType scaleType);

    QString dataSeriesName();
    void update(const PlotSettings &settings);    

protected slots:
    void onNewData(qint64 timestamp);

private:
    const DoubleSeries &series;
    QCustomPlot *plot;
    QCPGraph *graph;

    qint64 lastUpdate;
    double currentMin;
    double currentMax;

    PlotSettings::ScalingMode currentScalingMode;
    PlotSettings::ScaleType currentScaleType;

    void configureAppearance(QCPGraph *graph);
    void initialize(QCPGraph *graph, const DoubleSeries &series);

    void rescale(PlotSettings::ScalingMode scalingMode, PlotSettings::ScaleType scaleType);
    void scaleToRange(double lower, double upper, PlotSettings::ScaleType scaleType);

    void updateMetadata(double value);
    void updatePlot(PlotSettings::ScalingMode scalingMode);
};

#endif // INTERPOLATINGGRAPH_H
