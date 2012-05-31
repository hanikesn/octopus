#ifndef ABSTRACTDATASERIES_H
#define ABSTRACTDATASERIES_H

#include <QMap>
#include <QObject>

class Value;

class AbstractDataSeries : public QObject
{
    Q_OBJECT
public:
    AbstractDataSeries(const QString &deviceName, const QString &dataSeriesName, bool stateful);

    QString device() const;
    QString name() const;
    QString fullName() const;
    bool isStateful() const;

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
     * True if the recorded values refer to the state of the device, otherwise false.
     */
    bool stateful;
};

#endif // ABSTRACTDATASERIES_H
