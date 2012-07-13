#ifndef STRINGSERIES_H
#define STRINGSERIES_H

#include "datamodel/abstractdataseries.h"

#include <QObject>

/*
  A string series contains data of type STRING.
*/
class StringSeries : public AbstractDataSeries
{
    Q_OBJECT
public:
    StringSeries(DataProvider &dp, const QString& deviceName, const QString& dataSeriesName, Data::Properties properties);

    /**
     * Visitor pattern.
     */
    void accept(DataSeriesVisitor *v);

    /**
     * Reimplemented from AbstractDataSeries.
     */
    void addData(qint64 timestamp, const Value &value);

    /**
     * @return A map of all timestamps and their data.
     */
    QMap<qint64, QString> getData() const;

    /**
     * @return A map of timestamps and the respective data for all timestamps t for which begin <= t <= end.
     */
    QMap<qint64, QString> getData(qint64 begin, qint64 end) const;
};

#endif // STRINGSERIES_H
