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

    auto const& prefix = QString::fromUtf8(msg.getSender().c_str()) + ".";

    foreach(auto const& p, msg.getContent()) {
        emit onNewData(timestamp, prefix + fromStdString(p.first), Value(p.second));
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
        auto const& sender = fromStdString(d.getSender());
        std::for_each(c.begin(), c.end(),
            [this, &sender](EI::DataSeriesInfoMap::value_type const& p)
        {
            emit onNewDataSeries(
                          sender,
                          fromStdString(p.first.c_str()),
                          convert(p.second.getProperties())
                );
        });
        knownSenders.insert(d.getSender());
    }
}
