#include "gui/discretegraph.h"

DiscreteGraph::DiscreteGraph(const StringSeries &s) :
    series(s)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
}

void DiscreteGraph::onNewData(qint64 timestamp)
{

}
