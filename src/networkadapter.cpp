#include "networkadapter.h"

#include "value.h"

#include <algorithm>

NetworkAdapter::NetworkAdapter()
    : receiver(EI::StringMap())
{
    receiver.addCommunicationListener(this);
    receiver.addDataListener(this);

    receiver.discoverSenders();
    lastDiscoverSent = startTime = Clock::now();
}

NetworkAdapter::~NetworkAdapter()
{
    receiver.removeDataListener(this);
    receiver.removeCommunicationListener(this);
}

void NetworkAdapter::onMessage(EI::DataMessage msg)
{
    Clock::time_point now = Clock::now();

    if(knownSenders.find(msg.getSender()) == knownSenders.end()
            && (now - lastDiscoverSent) < boost::chrono::seconds(5)) {
        receiver.discoverSenders();
        lastDiscoverSent = now;
    }

    qint64 timestamp = boost::chrono::duration_cast<boost::chrono::microseconds>(now - startTime).count();

    foreach(auto const& p, msg.getContent()) {
        emit onNewData(timestamp, QString::fromUtf8(std::string(msg.getSender() + "." + p.first).c_str()), Value(p.second));
    }
}

static Data::Properties convert(EI::DataSeriesInfo::Properties prop)
{
    Data::Properties result;

    if(prop & EI::DataSeriesInfo::INTERPOLATABLE)
        result |= Data::INTERPOLATABLE;

    if(prop & EI::DataSeriesInfo::STATEFUL)
        result |= Data::STATEFUL;

    return result;
}

void NetworkAdapter::onMessage(EI::Message const& msg)
{
    if(msg.getMsgType()==EI::DescriptionMessage::IDENTIFIER)
    {
        auto const& d = dynamic_cast<EI::DescriptionMessage const&>(msg);
        auto const& c = d.getDescription().getDataSeries();
        std::for_each(c.begin(), c.end(),
            [this, &d](EI::DataSeriesInfoMap::value_type const& p)
        {
            emit onNewDataSeries(
                QString::fromUtf8(d.getSender().c_str()),
                QString::fromUtf8(p.first.c_str()),
                convert(p.second.getProperties())
                );
        });
        knownSenders.insert(d.getSender());
    }
}
