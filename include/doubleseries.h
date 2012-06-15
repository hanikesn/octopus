#ifndef DOUBLESERIES_H
#define DOUBLESERIES_H

#include "abstractdataseries.h"
#include "common.h"

#include <QObject>

class DoubleSeries : public AbstractDataSeries
{
    Q_OBJECT
public:
    DoubleSeries(const DataProvider &dp, const QString &deviceName, const QString &name, Data::Properties properties);

    /**
     * Visitor pattern.
     */
    void accept(DataSeriesVisitor *v);
    
    void addData(qint64 timeStamp, const Value &value);

    /**
     * @return The value for the given timestamp.
     */
    double getData(qint64 timestamp) const;

    /**
     * @return A map of all timestamps and the respective data.
     */
    QMap<qint64, double> getData() const;

    /**
     * @return A map of timestamps and the respective data for all timestamps t for which begin <= t <= end.
     */
    QMap<qint64, double> getData(qint64 begin, qint64 end) const;
};

#endif // DOUBLESERIES_H
