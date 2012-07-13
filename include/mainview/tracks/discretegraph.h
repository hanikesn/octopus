/*
  A discrete graph shows discrete events as impulses with a label describing the event.
*/

#ifndef DISCRETEGRAPH_H
#define DISCRETEGRAPH_H

#include "mainview/tracks/graph.h"

class StringSeries;
class QCustomPlot;
class QCPGraph;
class QCPItemAnchor;
class QCPItemText;
class QCPItemTracer;

class DiscreteGraph : public Graph
{
    Q_OBJECT
public:
    explicit DiscreteGraph(QCustomPlot *plot, const StringSeries &s);

    QCPGraph* getGraph();

    QString dataSeriesName();
    void update(const PlotSettings &settings);

protected slots:
    void onNewData(qint64 timestamp);
    void onOffsetChanged();

private:
    const StringSeries &series;
    QCustomPlot *plot;
    QCPGraph *graph;

    /**
     * The time stamp (in µs) of the graph's last update.
     */
    qint64 lastUpdate;

    /**
     * Configures the appearance of the graph on the plot.
     */
    void configureAppearance(QCPGraph *graph);

    /**
     * Initializes the graph with the data from its data series.
     */
    void initialize(QCPGraph *graph, const StringSeries &series);

    /**
     * Adds an event to the graph at the given timestamp.
     * @param timestamp The event's timestamp.
     * @param str The label for the event.
     */
    void addData(qint64 timestamp, QString const& str);

    /**
     * Adds a tracer to the graph, e.g. for attaching a label.
     * @param graphKey The key axis coordinate to which the tracer is to be attached.
     * @param size The size of the dot representing the tracer in the graph.
     */
    QCPItemTracer* addTracer(double graphKey, double size);

    /**
     * Adds a label to the graph.
     * @param text The text for the label
     * @param parentAnchor The anchor to which the label will be attached
     */
    QCPItemText* addLabel(const QString& text, QCPItemAnchor *parentAnchor);
};

#endif // DISCRETEGRAPH_H
