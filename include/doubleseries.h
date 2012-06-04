#ifndef DOUBLESERIES_H
#define DOUBLESERIES_H

#include "abstractdataseries.h"
#include "common.h"

#include <QObject>

class DoubleSeries : public AbstractDataSeries
{
    Q_OBJECT
public:
    DoubleSeries(const QString &deviceName, const QString &name, Data::Properties properties);

    /**
     * Visitor pattern.
     */
    void accept(DataSeriesVisitor *v);
    
    void addData(qint64 timeStamp, const Value &value);

    /**
     * Convenience function.
     */
    QList<double> getData(qint64 timestamp) const;

    /**
     * @return A sorted list of the data for all timestamps t for which begin <= t <= end.
     */
    QList<double> getData(qint64 begin, qint64 end) const;

private:
    /**
     * Map of timestamps and respective values
     */
    QMap<qint64, double> values;
};

#endif // DOUBLESERIES_H
