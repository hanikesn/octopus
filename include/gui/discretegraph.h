#ifndef DISCRETEGRAPH_H
#define DISCRETEGRAPH_H

#include "stringseries.h"
#include "gui/graph.h"

class DiscreteGraph : public Graph
{
public:
    explicit DiscreteGraph(const StringSeries &s);

private:
    const StringSeries &series;
};

#endif // DISCRETEGRAPH_H
