#include "PortBindingManager.h"

PortBindingManager::PortBindingManager(QObject *parent) :
    QObject {parent}
{}

void
PortBindingManager::bind(const PortPtr_t &port1, const PortPtr_t &port2)
{
    connect(port1.get(), &Port::packetSent, port2.get(), &Port::receivePacket);
    connect(port2.get(), &Port::packetSent, port1.get(), &Port::receivePacket);

    m_bindings[port1].append(port2);
    m_bindings[port2].append(port1);
}

bool
PortBindingManager::unbind(const PortPtr_t &port1, const PortPtr_t &port2)
{}
