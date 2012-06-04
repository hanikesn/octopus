/**
 * Implementation of the visitor pattern.
 */

#ifndef VISITOR_H
#define VISITOR_H

class DoubleSeries;
class StringSeries;

class DataSeriesVisitor {
public:
    virtual ~DataSeriesVisitor() {}

    virtual void addGraph(DoubleSeries *s) = 0;
    virtual void addGraph(StringSeries *s) = 0;

protected:
    DataSeriesVisitor() {}
};

class Visitable {
public:
    virtual ~Visitable() {}

    virtual void accept(DataSeriesVisitor *v) = 0;

protected:
    Visitable() {}
};

#endif // VISITOR_H
