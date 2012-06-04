#include "gui/interpolatinggraph.h"

InterpolatingGraph::InterpolatingGraph(const DoubleSeries &d) :
    series(d)
{
    connect(&series, SIGNAL(newData(qint64)), this, SLOT(onNewData(qint64)));
}

void InterpolatingGraph::onNewData(qint64 timestamp)
{

}
