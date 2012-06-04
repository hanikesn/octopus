#ifndef DISCRETEGRAPH_H
#define DISCRETEGRAPH_H

#include "stringseries.h"
#include "gui/graph.h"

class DiscreteGraph : public Graph
{
    Q_OBJECT
public:
    explicit DiscreteGraph(const StringSeries &s);

private:
    const StringSeries &series;

protected slots:
    void onNewData(qint64 timestamp);
};

#endif // DISCRETEGRAPH_H
