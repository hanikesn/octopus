#ifndef STRINGSERIES_H
#define STRINGSERIES_H

#include "abstractdataseries.h"
#include "common.h"

#include <QObject>

class StringSeries : public AbstractDataSeries
{
    Q_OBJECT
public:
    StringSeries(const QString& deviceName, const QString& dataSeriesName, Data::Properties properties);

    void addData(qint64 timeStamp, const Value &value);

    /**
     * Convenience function.
     */
    QList<QString> getData(qint64 timeStamp) const;

    /**
     * @return A sorted list of the data for all timestamps t for which begin <= t <= end.
     */
    QList<QString> getData(qint64 begin, qint64 end) const;

private:
    /**
     * Map of timestamps and respective values
     */
    QMap<qint64, QString> values;
};

#endif // STRINGSERIES_H
