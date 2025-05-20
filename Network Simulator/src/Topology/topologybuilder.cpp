#include "topologybuilder.h"
#include "../Node/networknode.h"
#include "../IP/IP.h"
#include <math.h>
#include <QVector>

TopologyBuilder::TopologyBuilder(QObject *parent) :
    QObject {parent}
{}

QVector<Router*> TopologyBuilder::createMeshTopology(PortBindingManager *&portBindingManager, int asId, int dhcp, QSet<int> brokens, int firstNode, int nodeCount, const QString &routing) {
    QVector<Router*> routers;
    int k = int(sqrt(nodeCount));

    for(int i = 0; i < k; i++) {
        for(int j = 0; j < k; j++) {
            int currId = firstNode + k * i + j;
            bool isDhcp = true; //(currId == dhcp);
            bool isBroken = brokens.contains(currId);
            Router *newRouter = new Router(currId, isDhcp, isBroken, asId, routing);

            if(i != 0) {
                PortPtr_t newPort = QSharedPointer<Port>::create("up");
                newRouter->m_ports["up"] = newPort;
                connect(newPort.get(), &Port::packetReceived, newRouter, &Router::receivePacket);
            }
            if(i != (k - 1)) {
                PortPtr_t newPort = QSharedPointer<Port>::create("down");
                newRouter->m_ports["down"] = newPort;
                connect(newPort.get(), &Port::packetReceived, newRouter, &Router::receivePacket);
            }
            if(j != 0) {
                PortPtr_t newPort = QSharedPointer<Port>::create("left");
                newRouter->m_ports["left"] = newPort;
                connect(newPort.get(), &Port::packetReceived, newRouter, &Router::receivePacket);
            }
            if(j != (k - 1)) {
                PortPtr_t newPort = QSharedPointer<Port>::create("right");
                newRouter->m_ports["right"] = newPort;
                connect(newPort.get(), &Port::packetReceived, newRouter, &Router::receivePacket);
            }
            routers.append(newRouter);
        }
    }

    for(int i = 0; i < k; i++) {
        for(int j = 0; j < k - 1; j++){
            portBindingManager->bind(routers[k * i + j]->m_ports["right"], routers[k * i + j + 1]->m_ports["left"]);
            portBindingManager->bind(routers[k * j + i]->m_ports["down"], routers[k * (j + 1) + i]->m_ports["up"]);
        }
    }

    return routers;
}


QVector<Router*> TopologyBuilder::createRingstarTopology(PortBindingManager *&portBindingManager, int asId, int dhcp, QSet<int> brokens, int firstNode, int nodeCount, const QString &routing) {
    QVector<Router*> routers;

    int centerNode = nodeCount - 1;
    for(int i = 0; i < nodeCount; i++) {
        int currId = firstNode + i;
        bool isDhcp = true; //(currId == dhcp);
        bool isBroken = brokens.contains(currId);
        Router *newRouter = new Router(currId, isDhcp, isBroken, asId, routing);

        if(i != centerNode) {
            newRouter->m_ports["right"] = QSharedPointer<Port>::create("right");;
            connect(newRouter->m_ports["right"].get(), &Port::packetReceived, newRouter, &Router::receivePacket);

            newRouter->m_ports["left"] = QSharedPointer<Port>::create("left");
            connect(newRouter->m_ports["left"].get(), &Port::packetReceived, newRouter, &Router::receivePacket);
        }
        routers.append(newRouter);
    }

    for(int i = 0; i < centerNode; i++) {
        if(i == centerNode - 1) {
            portBindingManager->bind(routers[i]->m_ports["right"], routers[0]->m_ports["left"]);
        } else {
            portBindingManager->bind(routers[i]->m_ports["right"], routers[i + 1]->m_ports["left"]);
        }

        if(i % 2 == 0) {
            QString ringNode = QString::number(routers[i]->getId());
            routers[centerNode]->m_ports[ringNode] = QSharedPointer<Port>::create(ringNode);
            connect(routers[centerNode]->m_ports[ringNode].get(), &Port::packetReceived, routers[centerNode], &Router::receivePacket);

            routers[i]->m_ports["center"] = QSharedPointer<Port>::create("center");
            connect(routers[i]->m_ports["center"].get(), &Port::packetReceived, routers[i], &Router::receivePacket);

            portBindingManager->bind(routers[centerNode]->m_ports[ringNode], routers[i]->m_ports["center"]);
        }
    }


    return routers;
}

