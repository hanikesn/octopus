#ifndef VISITOR_H
#define VISITOR_H

class DoubleSeries;
class StringSeries;

/*
  Implementation of the visitor pattern.
*/
class DataSeriesVisitor {
public:
    virtual ~DataSeriesVisitor() {}

    virtual void addGraph(const DoubleSeries &s) = 0;
    virtual void addGraph(const StringSeries &s) = 0;

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
