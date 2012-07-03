#ifndef GRAPH_H
#define GRAPH_H

#include "plotsettings.h"

#include <QObject>
#include <QtCore/QIntegerForSize>

class QCPGraph;

class Graph : public QObject
{
    Q_OBJECT
public:
    Graph(QObject* parent) : QObject(parent) {}
    virtual ~Graph() {}

    virtual QCPGraph* getGraph() = 0;
    virtual PlotSettings::ScaleType getScaleType() const {return PlotSettings::NOT_SCALABLE;}

    virtual QString dataSeriesName() = 0;
    virtual void update(const PlotSettings &settings) = 0;

protected:
    PlotSettings::ScaleType currentScaleType;

protected slots:
    virtual void onNewData(qint64 timestamp) = 0;
    virtual void onOffsetChanged() = 0;
};

#endif // GRAPH_H
