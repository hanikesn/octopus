#ifndef NETWORKADAPTER_H
#define NETWORKADAPTER_H

#include "common.h"
#include "value.h"

#include <EIToolkit.h>
#include <QObject>
#include <set>
#include <ctime>
#include <boost/chrono.hpp>

class Value;
class QString;

class NetworkAdapter : public QObject, public EI::DataListener, public EI::CommunicationListener
{
    Q_OBJECT
public:
    NetworkAdapter();
    ~NetworkAdapter();

    virtual void onMessage(EI::DataMessage msg);
    virtual void onMessage(EI::Message const& msg);

signals:
    void onNewDataSeries(QString deviceName, QString dataSeriesName, Data::Properties properties);

    void onNewData(qint64 timestamp, QString fullDataSeriesName, Value value);

private:
    EI::Receiver receiver;
    std::set<std::string> knownSenders;

    typedef boost::chrono::high_resolution_clock Clock;

    Clock::time_point lastDiscoverSent;

    Clock::time_point startTime;
};

#endif
