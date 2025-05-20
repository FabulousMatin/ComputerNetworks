#include "autonomoussystem.h"
#include "../Topology/topologybuilder.h"
#include "../Routing/routingprotocol.h"

void AutonomousSystem::buildTopology(const ApplicationContext::AutonomousSystemConfig& asConf, EventsCoordinator *&eventsCoordinator, PortBindingManager *&portBindingManager, const QString &routingProtocl) {
    TopologyBuilder *tp = new TopologyBuilder();

    if(asConf.topologyType == "Mesh") {
        m_routers = tp->createMeshTopology(portBindingManager, m_id, asConf.dhcpServer, asConf.brokenRouters, asConf.firstNode, asConf.nodeCount, routingProtocl);
    } else if (asConf.topologyType == "RingStar") {
        m_routers = tp->createRingstarTopology(portBindingManager, m_id, asConf.dhcpServer, asConf.brokenRouters, asConf.firstNode, asConf.nodeCount, routingProtocl);
    }
    delete tp;

    for(int i = 0; i < m_routers.size(); i++) {
        connect(eventsCoordinator, &EventsCoordinator::clock, m_routers[i], &Router::nextClock);
    }

}


void AutonomousSystem::buildPcs(const ApplicationContext::AutonomousSystemConfig& asConf, EventsCoordinator *&eventsCoordinator, PortBindingManager *&portBindingManager) {
    for (const ApplicationContext::AutonomousSystemConfig::GatewayConfig &gateway : asConf.gateways) {
        for(int id : gateway.users) {
            PC *newPc = new PC(id, asConf.id);

            newPc->m_port = QSharedPointer<Port>::create("port");
            Router* router = getRouterById(gateway.node);

            QString routerToPc = "PC" + QString::number(id);

            router->m_ports[routerToPc] = QSharedPointer<Port>::create(routerToPc);
            connect(router->m_ports[routerToPc].get(), &Port::packetReceived, router, &Router::receivePacket);
            portBindingManager->bind(newPc->m_port, router->m_ports[routerToPc]);

            connect(eventsCoordinator, &EventsCoordinator::sendBuffer, newPc, &PC::getBuffer);
            connect(eventsCoordinator, &EventsCoordinator::clock, newPc, &PC::nextClock);
            connect(newPc->m_port.get(), &Port::packetReceived, newPc, &PC::receivePacket);
            m_pcs.append(newPc);
        }

    }
}


AutonomousSystem::AutonomousSystem(const ApplicationContext::SimulationConfig &config, const ApplicationContext::AutonomousSystemConfig& asConf, EventsCoordinator *&eventsCoordinator, PortBindingManager *&portBindingManager, Simulator *&simulator, QObject *parent) :
    QObject {parent}
{

    m_id = asConf.id;
    m_dhcpServer = asConf.dhcpServer;

    this->buildTopology(asConf, eventsCoordinator, portBindingManager, config.routingProtocol);
    this->buildPcs(asConf, eventsCoordinator, portBindingManager);

    for(int i = 0; i < m_routers.size(); i++) {
        connect(simulator, &Simulator::changeState, m_routers[i], &Router::stateChanged);
        connect(m_routers[i], &NetworkNode::ipDone, simulator, &Simulator::dchpDone);
        connect(m_routers[i]->getRoutingProtocol(), &RoutingProtocol::routingDone, simulator, &Simulator::routingDone);

        connect(m_routers[i], &Router::sendForwardingStats, simulator, &Simulator::forwardingStatsReceived);
        connect(m_routers[i], &Router::sendRoutingTable, simulator, &Simulator::routingTableReceived);

    }

    for(int i = 0; i < m_pcs.size(); i++) {
        connect(simulator, &Simulator::changeState, m_pcs[i], &NetworkNode::stateChanged);
        connect(m_pcs[i], &NetworkNode::ipDone, simulator, &Simulator::dchpDone);

        connect(m_pcs[i], &PC::sendCycleStats, simulator, &Simulator::cycleStatsReceived);
        connect(m_pcs[i], &PC::sendPath, simulator, &Simulator::pathReceived);
    }
}


QVector<int> AutonomousSystem::getPcIds() {
    QVector<int> ids;
    for (int i = 0; i < m_pcs.size(); i++) {
        ids.append(m_pcs[i]->getId());
    }

    return ids;
}

Router* AutonomousSystem::getRouterById(int id) {
    for(int i = 0; i < m_routers.size(); i++) {
        if(m_routers[i]->getId() == id) {
            return m_routers[i];
        }
    }

    return nullptr;
}


int AutonomousSystem::getId() {
    return m_id;
}
