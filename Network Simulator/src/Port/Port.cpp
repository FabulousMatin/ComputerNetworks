#include "Port.h"

Port::Port(const QString &id, QObject *parent) :
    QObject {parent},
    m_id(id)
{}

Port::~Port() {}

void Port::sendPacket(const PacketPtr_t &data)
{
    ++m_numberOfPacketsSent;

    Q_EMIT packetSent(data);
}

void Port::receivePacket(const PacketPtr_t &data) {
    Q_EMIT packetReceived(data, m_id);
}


QString Port::getPortId() {
    return m_id;
}
