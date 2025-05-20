#include "network.h"
#include "../PortBindingManager/PortBindingManager.h"

Network::Network(const ApplicationContext::SimulationConfig config, EventsCoordinator *&eventCoordinator, Simulator* &simulator, QObject *parent) :
    QObject {parent}
{
    PortBindingManager *portBindingManager = new PortBindingManager();

    QVector<int> pcIds;

    for(const ApplicationContext::AutonomousSystemConfig& asConf : config.autonomousSystems) {
        AutonomousSystem *new_as = new AutonomousSystem(config, asConf, eventCoordinator, portBindingManager, simulator);

        for (const ApplicationContext::AutonomousSystemConfig::ConnectionConfig &asConnection : asConf.connections) {
            AutonomousSystem* otherAs = nullptr;
            for(int i = 0; i < m_autonomousSystems.size(); i++) {
                if(m_autonomousSystems[i]->getId() == asConnection.id)
                    otherAs = m_autonomousSystems[i];
            }

            for(const ApplicationContext::AutonomousSystemConfig::ConnectionConfig::GatewayPairConfig &asPair : asConnection.gatewayPairs) {
                Router* thisRouter = new_as->getRouterById(asPair.gateway);
                Router* otherRouter = otherAs->getRouterById(asPair.connectTo);

                QString portId = "AS" + QString::number(otherRouter->getId());
                thisRouter->m_ports[portId] = QSharedPointer<Port>::create(portId);
                connect(thisRouter->m_ports[portId].get(), &Port::packetReceived, thisRouter, &Router::receivePacket);

                portId = "AS" + QString::number(new_as->getId());
                otherRouter->m_ports[portId] = QSharedPointer<Port>::create(portId);
                connect(otherRouter->m_ports[portId].get(), &Port::packetReceived, otherRouter, &Router::receivePacket);

                portBindingManager->bind(
                  thisRouter->m_ports["AS" + QString::number(otherRouter->getId())],
                  otherRouter->m_ports["AS" + QString::number(new_as->getId())]
                  );
            }
        }

        pcIds += new_as->getPcIds();
        m_autonomousSystems.append(new_as);
    }

    delete portBindingManager;

    eventCoordinator->addPcIds(pcIds);

}
