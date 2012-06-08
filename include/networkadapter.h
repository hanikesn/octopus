#ifndef NETWORKADAPTER_H
#define NETWORKADAPTER_H

#include "common.h"

#include <EIToolkit.h>
#include <QObject>
#include <set>
#include <ctime>
#include <boost/chrono.hpp>

class Value;
class QString;

class NetworkAdapter : QObject, public EI::DataListener, public EI::CommunicationListener
{
    Q_OBJECT
public:
    NetworkAdapter();
    ~NetworkAdapter();

    virtual void onMessage(EI::DataMessage msg);
    virtual void onMessage(EI::Message const& msg);

signals:
    void onNewDataSeries(const QString &deviceName, const QString &dataSeriesName, Data::Properties properties);

    void onNewData(qint64 timestamp, const QString &fullDataSeriesName, const Value &value);

private:
    EI::Receiver receiver;
    std::set<std::string> knownSenders;

    typedef boost::chrono::high_resolution_clock Clock;

    Clock::time_point startTime;
};

#endif