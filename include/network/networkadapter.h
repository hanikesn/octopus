#ifndef NETWORKADAPTER_H
#define NETWORKADAPTER_H

#include "common/common.h"
#include "common/metadata.h"
#include "common/value.h"

#include <EIToolkit.h>

#include <boost/chrono.hpp>
#include <ctime>
#include <set>
#include <QObject>

class Value;
class QString;

/**
 * @brief This class handles everything related to the EIToolkit
 */
class NetworkAdapter : public QObject, public EI::DataListener, public EI::CommunicationListener
{
    Q_OBJECT
public:
    NetworkAdapter(QObject* parent);
    ~NetworkAdapter();

    typedef boost::chrono::high_resolution_clock Clock;

    void discoverSenders();

    virtual void onMessage(EI::DataMessage msg);
    virtual void onMessage(EI::Message const& msg);    

    Clock::time_point getStartTime();

signals:
    void newSender(EIDescriptionWrapper);

    void newData(qint64 timestamp, QString fullDataSeriesName, Value value);

private:
    EI::Receiver receiver;
    std::set<std::string> knownSenders;

    Clock::time_point lastDiscoverSent;

    Clock::time_point startTime;
};

#endif
