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
     * The default scale type to be used for plotting the data.
     */
    PlotSettings::ScaleType defaultScaleType;

    QString device() const;
    QString name() const;
    QString fullName() const;
    Data::Properties properties() const;
    qint64 offset() const;

    virtual void addData(qint64 timeStamp, const Value &value) = 0;
    virtual void setOffset(qint64 newOffset);

signals:
    void newData(qint64 timestamp);
    void offsetChanged();
    void illegalValueType();

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
