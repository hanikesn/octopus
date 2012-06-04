#ifndef INTERPOLATINGGRAPH_H
#define INTERPOLATINGGRAPH_H

#include "doubleseries.h"
#include "gui/graph.h"

class InterpolatingGraph : public Graph
{
public:
    explicit InterpolatingGraph(const DoubleSeries &d);

private:
    const DoubleSeries &series;
};

#endif // INTERPOLATINGGRAPH_H
