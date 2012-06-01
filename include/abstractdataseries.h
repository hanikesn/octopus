#ifndef ABSTRACTDATASERIES_H
#define ABSTRACTDATASERIES_H

#include "common.h"

#include <QMap>
#include <QObject>

class Value;

class AbstractDataSeries : public QObject
{
    Q_OBJECT
public:
    AbstractDataSeries(const QString &deviceName, const QString &dataSeriesName, Data::Properties properties);

    QString device() const;
    QString name() const;
    QString fullName() const;
    Data::Properties properties() const;

    virtual void addData(qint64 timeStamp, const Value &value) = 0;

signals:
    void illegalValueType();

private:
    /**
     * The device name uniquely identifies the device this data series comes from, e.g. "kinect-7".
     */
    QString deviceName;

    /**
     * The name of the data series uniquely identifies this data series. It is a string of the format <data series name>.<component name>.<...>, e.g. "leftKnee.x".
     */
    QString dataSeriesName;

    /**
     * Stores a combination of data property flags.
     */
    Data::Properties props;
};

#endif // ABSTRACTDATASERIES_H
