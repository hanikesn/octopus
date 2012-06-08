#include "networkadapter.h"

#include <algorithm>

NetworkAdapter::NetworkAdapter()
    : receiver(EI::StringMap())
{
    receiver.addCommunicationListener(*this);
    receiver.addDataListener(*this);

    receiver.discoverSenders();
    startTime = Clock::now();
}

NetworkAdapter::~NetworkAdapter()
{
    receiver.removeDataListener(*this);
    receiver.removeCommunicationListener(*this);
}

void NetworkAdapter::onMessage(EI::DataMessage msg)
{
    Clock::time_point now = Clock::now();
    //qint64 timestamp = (current_time - startTime) * 1,000,000 CLOCKS_PER_SEC;
    // TODO steffen rate limiting
    if(knownSenders.find(msg.getSender()) == knownSenders.end())
        receiver.discoverSenders();

    //emit onNewData();    
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
        auto const& desc = d.getDescription();
        auto const& c = desc.getDataSeries();
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
