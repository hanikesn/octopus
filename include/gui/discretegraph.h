#ifndef DISCRETEGRAPH_H
#define DISCRETEGRAPH_H

#include "gui/graph.h"

class StringSeries;
class QCustomPlot;
class QCPGraph;
class QCPItemAnchor;
class QCPItemTracer;
class QCPItemText;

class DiscreteGraph : public Graph
{
    Q_OBJECT
public:
    explicit DiscreteGraph(QCustomPlot *plot, const StringSeries &s);

    QCPGraph* getGraph();

    void setScaleType(PlotSettings::ScaleType /*scaleType*/) {}

    QString dataSeriesName();
    void update(const PlotSettings &settings);

protected slots:
    void onNewData(qint64 timestamp);

private:
    const StringSeries &series;
    QCustomPlot *plot;
    QCPGraph *graph;

    qint64 lastUpdate;

    void configureAppearance(QCPGraph *graph);
    void initialize(QCPGraph *graph, const StringSeries &series);

    void addData(qint64 timestamp, QString const& str);
    QCPItemTracer* addTracer(double graphKey, double size);
    QCPItemText* addLabel(const QString& text, QCPItemAnchor *parentAnchor);
};

#endif // DISCRETEGRAPH_H
