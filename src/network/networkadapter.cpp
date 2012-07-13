#include "network/networkadapter.h"

#include "common/value.h"

#include <algorithm>

NetworkAdapter::NetworkAdapter(QObject* parent) :
    QObject(parent),
    receiver(EI::StringMap()),
    startTime(Clock::now())
{
    knownSenders.insert("Receiver");
    receiver.addCommunicationListener(this);
    receiver.addDataListener(this);
}

NetworkAdapter::~NetworkAdapter()
{
    receiver.removeDataListener(this);
    receiver.removeCommunicationListener(this);
}

void NetworkAdapter::discoverSenders()
{
    receiver.discoverSenders();
    lastDiscoverSent = Clock::now();
}

void NetworkAdapter::onMessage(EI::DataMessage msg)
{
    Clock::time_point now = Clock::now();

    if(knownSenders.find(msg.getSender()) == knownSenders.end()
            && (now - lastDiscoverSent) > boost::chrono::seconds(5)) {
        receiver.discoverSenders();
        lastDiscoverSent = now;
    }

    qint64 timestamp = boost::chrono::duration_cast<boost::chrono::microseconds>(now - startTime).count();

    const QString prefix = fromStdString(msg.getSender()) + ".";

    foreach(auto const& p, msg.getContent()) {
        emit newData(timestamp, QString(prefix +  fromStdString(p.first)), Value(p.second));
    }
}

void NetworkAdapter::onMessage(EI::Message const& msg)
{
    if(msg.getMsgType()==EI::DescriptionMessage::IDENTIFIER)
    {
        auto const& d = dynamic_cast<EI::DescriptionMessage const&>(msg);
        auto const& desc = d.getDescription();
        emit newSender(desc);
        knownSenders.insert(d.getSender());
    }
}

NetworkAdapter::Clock::time_point NetworkAdapter::getStartTime()
{
    return startTime;
}
