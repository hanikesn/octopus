/*
  All data series need to derive from this class.
  Essentially, a data series provides access to its data.
  It also contains metainformation on its data, such as the name of the
device sending the data and the name of the data series itself, as well
as information on the properties of its data.
*/

#ifndef ABSTRACTDATASERIES_H
#define ABSTRACTDATASERIES_H

#include "metadata.h"
#include "plotsettings.h"
#include "visitor.h"

class Value;
class DataProvider;

#include <QObject>

class AbstractDataSeries : public QObject, public Visitable
{
    Q_OBJECT
public:
    virtual ~AbstractDataSeries() {}
    AbstractDataSeries(DataProvider &dp, const QString &deviceName, const QString &dataSeriesName, Data::Properties properties);

    /**
     * Visitor pattern.
     */
    virtual void accept(DataSeriesVisitor *v) = 0;

    /**
     * @return The default scale type to be used for plotting the data.
     */
    PlotSettings::ScaleType defaultScaleType;

    /**
     * @return The device the data series belongs to.
     */
    QString device() const;

    /**
     * @return The name of the data series without the device prefix.
     */
    QString name() const;

    /**
     * @return The name of the data series with the device prefix.
     */
    QString fullName() const;

    /**
     * @return This data series' data properties.
     */
    Data::Properties properties() const;

    /**
     * @return The offset in microseconds that has been set for the data
     *      series. This value is added to each time stamp and can for
     *      example be used to compensate time lags in the signal transmission.
     */
    qint64 offset() const;

    virtual void addData(qint64 timeStamp, const Value &value) = 0;

    /**
     * Changes the data series' offset to the given value.
     */
    virtual void setOffset(qint64 newOffset);

signals:
    /**
     * Emitted when the data series has new data.
     */
    void newData(qint64 timestamp);

    /**
     * Emitted when the data series' offset has been changed.
     */
    void offsetChanged();

protected:
    DataProvider &dp;

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
