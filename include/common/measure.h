#ifndef MEASURE_H
#define MEASURE_H

#include <boost/chrono.hpp>

#include <QDebug>

class Measurement
{
    typedef boost::chrono::high_resolution_clock Clock;
    Q_DISABLE_COPY(Measurement)
public:
    Measurement(QString const& name) : name(name), start(Clock::now()) {}
    ~Measurement()
    {
        Clock::time_point end = Clock::now();
        qDebug() << name << ": " << boost::chrono::duration_cast<boost::chrono::duration<double, boost::milli> >(end - start).count() << "ms";
    }

private:
    const QString name;
    Clock::time_point start;
};

#define LOKI_CONCATENATE_DIRECT(s1, s2)  s1##s2
#define LOKI_CONCATENATE(s1, s2)         LOKI_CONCATENATE_DIRECT(s1, s2)
#define LOKI_ANONYMOUS_VARIABLE(str)     LOKI_CONCATENATE(str, __LINE__)

#if 0
    #define MEASURE(name)      ::Measurement LOKI_ANONYMOUS_VARIABLE(Measurement)(name)
#else
    #define MEASURE(name)
#endif

#endif // MEASURE_H
