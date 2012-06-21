#ifndef INTERPOLATINGGRAPH_H
#define INTERPOLATINGGRAPH_H

#include "gui/graph.h"

class PlotSettings;
class QCustomPlot;
class QCPGraph;
class DoubleSeries;

class InterpolatingGraph : public Graph
{
    Q_OBJECT
public:
    explicit InterpolatingGraph(QCustomPlot *plot, const DoubleSeries &d);

    QCPGraph* getGraph();

    QString dataSeriesName();
    void update(PlotSettings settings, bool respectScalingMode = true);

private:
    const DoubleSeries &series;
    QCustomPlot *plot;
    QCPGraph *graph;

    qint64 lastUpdate;
    int offset;

//    ScalingMode currentScalingMode;

    void configureAppearance(QCPGraph *graph);
    void initialize(QCPGraph *graph, const DoubleSeries &series, int offsetMicroSecs);

//    void rescale(ScaleType scaleType, ScalingMode scalingMode);
    void setOffset(int microSecs);

protected slots:
    void onNewData(qint64 timestamp);
};

#endif // INTERPOLATINGGRAPH_H
