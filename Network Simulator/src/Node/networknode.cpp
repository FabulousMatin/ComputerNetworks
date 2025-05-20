#include "networknode.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include "../applicationcontext.h"
#include <conio.h>
#include <iostream>

NetworkNode::NetworkNode(int id, int asId, QThread *parent) :
    QThread {parent},
    m_id(id),
    m_asId(asId)
{}

void dumpForwarded(int forwarded, int lost, const QString& fileName) {
    QJsonObject jsonObj;
    jsonObj["forwarded"] = forwarded;
    jsonObj["lost"] = lost;

    QJsonDocument jsonDoc(jsonObj);
    QByteArray jsonData = jsonDoc.toJson();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << fileName;
        return;
    }

    file.write(jsonData);
    file.close();
}


void dumpCycles(QString from, QString to, int waiting, int total, const QString& fileName) {
    QJsonObject jsonObj;
    jsonObj["waiting"] = waiting;
    jsonObj["total"] = total;
    jsonObj["from"] = from;
    jsonObj["to"] = to;

    QJsonDocument jsonDoc(jsonObj);
    QByteArray jsonData = jsonDoc.toJson();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << fileName;
        return;
    }

    file.write(jsonData);
    file.close();
}


void dumpPath(QString from, QString to, const QList<QString>& list, const QString& fileName) {
    QJsonArray jsonArray;
    for (const QString& item : list) {
        jsonArray.append(item);
    }

    QJsonObject jsonObj;
    jsonObj["from"] = from;
    jsonObj["to"] = to;
    jsonObj["path"] = jsonArray;


    QJsonDocument jsonDoc(jsonObj);
    QByteArray jsonData = jsonDoc.toJson();

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << fileName;
        return;
    }

    file.write(jsonData);
    file.close();
}

PC::PC(int id, int asId, QThread *parent) :
    NetworkNode {id, asId, parent}
{
    if(m_id == 24) {
        // Total packets: 22828
        m_receiverBuffer.resize(22828);
        for(int i = 25; i <= 38; i++) {
            m_tcpReceiveInfo[i] = new TCPNewRenoReceiver();
        }
    }
    else {
        m_tcpSenderInfo = new TCPNewRenoSender();
    }
}

RoutingProtocol* Router::getRoutingProtocol() {
    return m_routingProtocol;
}

Router::Router(int id, bool isDhcp, bool isBroken, int asId, QString routing, QThread *parent) :
    NetworkNode {id, asId, parent},
    m_buffer(new Buffer()),
    m_isDhcp(isDhcp),
    m_isBroken(isBroken)

{
    if(routing == "OSPF") {
        m_routingProtocol = new OSPF();
        m_ospfState = OSPFState::Hello;
    }
    else {
        m_routingProtocol = new RIP();
        m_ripState = RIPState::INIT;
    }
    if(isDhcp) {
        m_ipv4 = QSharedPointer<IPv4>::create("192.168." + QString::number(m_asId) + "." + QString::number(m_id));
        m_dhcpState = DHCPState::IP_DONE;
        m_routingProtocol->setIp(m_ipv4->toString());
    }

}

void NetworkNode::stateChanged(NETWORK::State newState) {
    m_networkState = newState;
}

void Router::stateChanged(NETWORK::State newState) {
    m_networkState = newState;

    // if(m_isBroken && m_networkState == NETWORK::State::ROUTING)
    //     m_networkState = NETWORK::State::IDLE;

    if(m_networkState == NETWORK::State::ROUTING || m_networkState == NETWORK::State::SIMULATION)
        m_buffer->clear();

    if(m_networkState == NETWORK::State::SIMULATION) {
        m_buffer->setMaxSize(2000);// ApplicationContext::getRouterBufferSize()
        Q_EMIT sendRoutingTable(m_ipv4->toString(), m_routingProtocol->getRoutingTable());
    }

    if(m_networkState == NETWORK::State::ANALYZE) {
        // QString fileName = "D:\\ComputerNetworks\\CN_CHomeworks_3\\data\\Forwarded\\Router" + QString::number(m_id) + ".json";
        // dumpForwarded(m_forwarded, m_lostPackets, fileName);

        Q_EMIT sendForwardingStats(m_ipv4->toString(), m_forwarded, m_lostPackets);
    }
}

void PC::getBuffer(int id, const QVector<PacketPtr_t>& buffer) {
    if(m_id == id)
        m_senderBuffer = buffer;
}

void PC::nextClock() {

    if(m_dhcpState == DHCPState::NO_IP) {
        m_port->sendPacket(Packet::createDiscoveryPacket(m_asId, QString::number(m_id)));
        m_dhcpState = DHCPState::DISCOVERY;
    }
    else if(m_dhcpState == DHCPState::REQUEST) {
        m_port->sendPacket(Packet::createReqPacket(m_asId, QString::number(m_id)));
        m_dhcpState = DHCPState::ACK;
    }
    else if(m_networkState == NETWORK::State::ROUTING) {
        m_port->sendPacket(Packet::createOspfHelloPacket(m_ipv4->toString()));
        // QMap<QString, RIP::RouteEntry> me;
        // me[m_ipv4->toString()] = {m_ipv4->toString(), 0};
        // m_port->sendPacket(Packet::createRIPPacket(me, m_ipv4->toString()));
        m_networkState = NETWORK::State::IDLE;
    }

    if(m_networkState == NETWORK::State::SIMULATION) {
        if(m_id == 24) {
            while (!m_receiverAckQueue.isEmpty()) {
                m_port->sendPacket(m_receiverAckQueue.dequeue());
            }
        }
        else {
            int sent = 0;
            while((sent < m_tcpSenderInfo->cwnd) && (m_tcpSenderInfo->head < m_senderBuffer.size())) {
                if(m_tcpSenderInfo->ackeds.contains(m_tcpSenderInfo->head)) {
                    m_tcpSenderInfo->head++;
                } else {
                    PacketPtr_t toSend = m_senderBuffer[m_tcpSenderInfo->head];
                    m_tcpSenderInfo->head++;
                    sent++;
                    toSend->setSourceIp(m_ipv4->toString());
                    m_port->sendPacket(toSend);
                }
            }
        }
    }
}

void PC::receivePacket(const PacketPtr_t &packet, const QString &portId) {
    if(m_dhcpState == DHCPState::DISCOVERY && packet->getType() == Packet::Type::Control && packet->getControlType() == NETWORK::PacketControlType::DHCPOffer) {
        QString id = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["ID"].toString();

        if(id == QString::number(m_id)) {
            QString ip = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["IP"].toString();
            m_ipv4 = QSharedPointer<IPv4>::create(ip);
            m_dhcpState = DHCPState::REQUEST;

        }
    }
    else if(m_dhcpState == DHCPState::ACK && packet->getType() == Packet::Type::Control && packet->getControlType() == NETWORK::PacketControlType::DHCPAcknowledge) {
        QString id = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["ID"].toString();
        if(id == QString::number(m_id)) {
            m_dhcpState = DHCPState::IP_DONE;
            qDebug() << m_id << m_ipv4->toString();
            Q_EMIT ipDone();
        }
    }

    else if(packet->getType() == Packet::Type::Control && packet->getControlType() == NETWORK::PacketControlType::TCPAck) {
        processTcpSender(packet);
    }

    else if (packet->getType() == Packet::Type::Data) {
        m_nRecievedPackets++;
        // QString fileName = "D:\\ComputerNetworks\\CN_CHomeworks_3\\data\\Path\\PC" + QString::number(m_id) + "_Packet" + QString::number(m_nRecievedPackets) + ".json";
        // dumpPath(packet->getSourceIp(), packet->getDestIp(), packet->getPath(), fileName);

        if(m_id == 24) {
            processTcpReceiver(packet);
        } else {
            processTcpSender(packet);
        }


        Q_EMIT sendCycleStats(packet->getWaitingCycles(), packet->getTotalCycles());
        Q_EMIT sendPath(packet->getPath());
    }
}

void saveMp3FromChunks(const QVector<QByteArray>& chunks, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Error opening file:" << file.errorString();
        return;
    }

    for (const QByteArray& chunk : chunks) {
        file.write(chunk);
    }
    file.close();
}

void printProgress(const QMap<int, TCPNewRenoReceiver*> &info, const QVector<QByteArray> &data) {
    QTextStream out(stdout);
    out << "\r"; // Move to the beginning of the line

        // system("cls");

    bool allDone = true;
    for (auto it = info.begin(); it != info.end(); ++it) {
        int percentage = qMin(it.value()->lastAck * 100 / 1629, 100);
        if(percentage < 100)
            allDone = false;
        int width = 50; // Width of progress bar
        int pos = width * percentage / 100;

        out << "PC " << it.key() << ": [";
        for (int i = 0; i < width; ++i) {
            if (i < pos) out << "#";
            else out << " ";
        }
        out << "] " << it.value()->lastAck << "/1629 (" << percentage << "%)\n";
    }
    out.flush();

    if(allDone) {
        saveMp3FromChunks(data, "D:\\ComputerNetworks\\CN_CHomeworks_3\\assets\\output.mp3");
    }
}



void PC::processTcpReceiver(const PacketPtr_t &packet) {
    int id = packet->getSourceIp().right(2).toInt();

    m_receiverBuffer[packet->getTcpDataSeq()] = packet->getPayload().toLocal8Bit();
    // qDebug() << packet->getPayload().toLocal8Bit().size();

    if(m_tcpReceiveInfo[id]->expectedSeqNum == packet->getSequenceNumber()) {
        m_tcpReceiveInfo[id]->lastAck = m_tcpReceiveInfo[id]->expectedSeqNum;
        m_tcpReceiveInfo[id]->expectedSeqNum++;
    }
    PacketPtr_t ack = Packet::createTcpAckPacket(QString::number(m_tcpReceiveInfo[id]->lastAck), packet->getSourceIp());
    ack->setSourceIp(m_ipv4->toString());
    m_receiverAckQueue.enqueue(ack);


    printProgress(m_tcpReceiveInfo, m_receiverBuffer);
}

void PC::processTcpSender(const PacketPtr_t &packet) {
    int ackNum = packet->getPayload().toInt();

    if(m_id == 25)
        // qDebug() << m_id << "Acked" << ackNum;

    if (ackNum == m_tcpSenderInfo->lastAcked) {
        m_tcpSenderInfo->duplicateAcks++;

        if (m_tcpSenderInfo->duplicateAcks == 3) {
            m_tcpSenderInfo->ssthresh = std::max(m_tcpSenderInfo->cwnd / 2, 1);
            m_tcpSenderInfo->cwnd = m_tcpSenderInfo->ssthresh;
            m_tcpSenderInfo->head = m_tcpSenderInfo->lastAcked + 1;
            m_tcpSenderInfo->inRecovery = true;
        } else if (m_tcpSenderInfo->duplicateAcks > 3 && m_tcpSenderInfo->inRecovery) {
            m_tcpSenderInfo->cwnd += 1;
        }
    } else {
        m_tcpSenderInfo->lastAcked = ackNum;
        m_tcpSenderInfo->ackeds.insert(ackNum);
        m_tcpSenderInfo->duplicateAcks = 0;

        if (m_tcpSenderInfo->inRecovery) {
            m_tcpSenderInfo->cwnd = m_tcpSenderInfo->ssthresh;
            m_tcpSenderInfo->inRecovery = false;
        } else if (m_tcpSenderInfo->cwnd < m_tcpSenderInfo->ssthresh) {
            m_tcpSenderInfo->cwnd *= 2;
        } else {
            m_tcpSenderInfo->cwnd += 1;
        }
    }

}

void Router::broadcast(const PacketPtr_t &packet) {
    for (const auto &key : m_ports.keys()) {
        m_ports[key]->sendPacket(PacketPtr_t::create(*packet.get()));
    }
}

bool Router::isDhcpPacket(NETWORK::PacketControlType t) {
    return (t == NETWORK::PacketControlType::DHCPDiscovery ||
            t == NETWORK::PacketControlType::DHCPOffer ||
            t == NETWORK::PacketControlType::DHCPRequest ||
            t == NETWORK::PacketControlType::DHCPAcknowledge);
}

bool Router::isOspfPacket(NETWORK::PacketControlType t) {
    return (t == NETWORK::PacketControlType::OSPFHello ||
            t == NETWORK::PacketControlType::OSPFLsa);
}
void Router::processDhcpPacket(const PacketPtr_t &packet) {
    int as = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["AS"].toInt();
    if(as != m_asId)
        return;

    if(m_isDhcp) {
        if(packet->getControlType() == NETWORK::PacketControlType::DHCPDiscovery) {
            QString id = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["ID"].toString();
            broadcast(Packet::createOfferPacket(m_asId, id));
        }
        else if(packet->getControlType() == NETWORK::PacketControlType::DHCPRequest) {
            QString id = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["ID"].toString();
            broadcast(Packet::createAckPacket(m_asId, id));
        }
    }

    else if(m_dhcpState == DHCPState::DISCOVERY && packet->getControlType() == NETWORK::PacketControlType::DHCPOffer) {
        QString id = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["ID"].toString();

        if(id == QString::number(m_id)) {
            QString ip = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["IP"].toString();
            m_ipv4 = QSharedPointer<IPv4>::create(ip);
            m_dhcpState = DHCPState::REQUEST;
            m_routingProtocol->setIp(m_ipv4->toString());
        }
        else
            broadcast(packet);
    }
    else if(m_dhcpState == DHCPState::ACK && packet->getControlType() == NETWORK::PacketControlType::DHCPAcknowledge) {
        QString id = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["ID"].toString();
        if(id == QString::number(m_id)) {
            m_dhcpState = DHCPState::IP_DONE;
            qDebug() << m_id << m_ipv4->toString();
            Q_EMIT ipDone();
        }
        else
            broadcast(packet);
    }
    else {
        QString id = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["ID"].toString();
        broadcast(packet);
    }
}

void Router::processOspfPacket(const PacketPtr_t &packet)  {
    if(packet->getControlType() == NETWORK::PacketControlType::OSPFHello) {
        QString ip = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["IP"].toString();
        m_routingProtocol->addNeighbor(ip, 1);
        m_osfpNeighborDone++;
        m_ipToPort[ip] = packet->getPortIdReceived();

        if(m_osfpNeighborDone == m_ports.size()) {
            m_routingProtocol->setIpToPort(m_ipToPort);
            m_ospfState = OSPFState::LSA;
            // qDebug() << m_id;
        }

    }

    else if(packet->getControlType() == NETWORK::PacketControlType::OSPFLsa) {
        QString from = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["FROM"].toString();
        QMap<QString, int> linkState;
        QJsonObject lsObject = QJsonDocument::fromJson(packet->getPayload().toUtf8()).object()["LINKSTATE"].toObject();
        for (auto it = lsObject.begin(); it != lsObject.end(); ++it) {
            linkState[it.key()] = it.value().toInt();
        }

        dynamic_cast<OSPF*>(m_routingProtocol)->receiveLSA(from, linkState);

        broadcast(packet);
    }


}

void Router::processRipPacket(const PacketPtr_t &packet) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(packet->getPayload().toUtf8());

    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray routesArray = jsonObject["RIP"].toArray();


    QMap<QString, RIP::RouteEntry> rip;
    for (const QJsonValue& value : routesArray) {

        QJsonObject routeObject = value.toObject();
        RIP::RouteEntry entry;
        entry.nextHop = routeObject["nextHop"].toString();
        entry.metric = routeObject["metric"].toInt();

        rip[routeObject["dest"].toString()] = entry;
    }

    m_ipToPort[packet->getSourceIp()] = packet->getPortIdReceived();


    dynamic_cast<RIP*>(m_routingProtocol)->receiveRIP(rip);


}

void Router::processPacket(const PacketPtr_t &packet) {
    if(isDhcpPacket(packet->getControlType())) {
        processDhcpPacket(packet);
    }
    else if(isOspfPacket(packet->getControlType())) {
        processOspfPacket(packet);
    }
    else if (packet->getControlType() == NETWORK::PacketControlType::RIP){
        processRipPacket(packet);
    }
}

void Router::forwardPacket(const PacketPtr_t &packet) {
    QString portId = m_routingProtocol->getNextHop(packet->getDestIp());
    if(m_ports.keys().contains(portId))
        m_ports[portId]->sendPacket(packet);
    else {
        qDebug() << "Error in sending to port" << portId << "from" << m_ipv4->toString();
    }
}

void Router::nextClock() {

    m_buffer->passOneCycle();

    if(m_dhcpState == DHCPState::NO_IP) {
        broadcast(Packet::createDiscoveryPacket(m_asId, QString::number(m_id)));
        m_dhcpState = DHCPState::DISCOVERY;
    }
    else if(m_dhcpState == DHCPState::REQUEST) {
        broadcast(Packet::createReqPacket(m_asId, QString::number(m_id)));
        m_dhcpState = DHCPState::ACK;
    }
    else if(m_networkState == NETWORK::State::ROUTING) {
        if(m_ospfState == OSPFState::Hello) {
            broadcast(Packet::createOspfHelloPacket(m_ipv4->toString()));
            m_ospfState = OSPFState::NEIGHBOR;
        }
        else if(m_ospfState == OSPFState::LSA) {
            broadcast(Packet::createLSAPacket(m_ipv4->toString(), m_routingProtocol->getNeighbors()));
            m_ospfState = OSPFState::LSASent;
        }

        else if(m_ripState == RIPState::INIT) {
            dynamic_cast<RIP*>(m_routingProtocol)->init();
            broadcast(Packet::createRIPPacket(dynamic_cast<RIP*>(m_routingProtocol)->getRoutingInfo(), m_ipv4->toString()));
            m_ripState = RIPState::UPDATE;
        }

        else if(m_ripState == RIPState::UPDATE) {
            if(m_ripNotUpdated == 20) {
                m_ripState = RIPState::RipDone;
                m_routingProtocol->setIpToPort(m_ipToPort);
                Q_EMIT m_routingProtocol->routingDone();
            }
            else {
                broadcast(Packet::createRIPPacket(dynamic_cast<RIP*>(m_routingProtocol)->getRoutingInfo(), m_ipv4->toString()));
                if(!dynamic_cast<RIP*>(m_routingProtocol)->isNeedToBeUpdate())
                    m_ripNotUpdated++;
            }
        }
    }

    while(!m_buffer->isEmpty()) {
        PacketPtr_t nextPacket = m_buffer->pop();
        if(!nextPacket)
            break;
        else if(nextPacket->getType() == Packet::Type::Control) {
            processPacket(nextPacket);
            break;
        }
        else {
            m_forwarded++;
            forwardPacket(nextPacket);
            // break;
        }
    }

    m_buffer->waitOneCycle();
}

void Router::receivePacket(const PacketPtr_t &packet, const QString &portId) {
    packet->setPortIdReceived(portId);
    for(const QString &p : packet->getPath()) {
        if(p == QString::number(m_id))
            return;
    }
    packet->decTtl();
    if(packet->getTtl() > 0) {
        packet->addPath(QString::number(m_id));
        int forwarded = m_buffer->push(packet);
        if(!forwarded && packet->getType() == Packet::Type::Data)
            m_lostPackets++;
    }
}

int NetworkNode::getId() {
    return m_id;
}
