#include "routingprotocol.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>


void dumpRoutingTable(const QMap<QString, RoutingProtocol::Route>& map, const QString& fileName) {
    QJsonObject jsonObj;
    for (auto it = map.begin(); it != map.end(); ++it) {
        QJsonObject r;
        r["ip"] = it.value().ip;
        r["port"] = it.value().port;

        jsonObj[it.key()] = r;
    }

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



RoutingProtocol::RoutingProtocol(QObject *parent) :
    QObject {parent}
{

}

QMap<QString, RoutingProtocol::Route> RoutingProtocol::getRoutingTable() const {
    return routingTable;
}

void RoutingProtocol::addNeighbor(const QString &neighborIp, int cost) {
    neighbors[neighborIp] = cost;
}

OSPF::OSPF(QObject *parent) :
    RoutingProtocol {parent}
{

}

void RoutingProtocol::setIp(const QString &ip) {
    m_ip = ip;
}

QMap<QString, int> RoutingProtocol::getNeighbors() {
    return neighbors;
}

int OSPF::getSeqNumber() {
    return seqNumber;
}

void OSPF::incSeqNumber() {
    seqNumber++;
}

void RoutingProtocol::setIpToPort(const QMap<QString, QString> &ipToPort) {
    m_ipToPort = ipToPort;
}

void RoutingProtocol::setPorts() {
    for (const auto &node : routingTable.keys()) {
        routingTable[node].port = m_ipToPort[routingTable[node].ip];
    }
}

QString RoutingProtocol::getNextHop(const QString &ip) {
    return routingTable[ip].port;
}

void OSPF::receiveLSA(const QString &from, const QMap<QString, int> &linkState) {
    if(lsdb.contains(from))
        return;

    lsdb.insert(from);
    topology[m_ip] = neighbors;
    topology[from] = linkState;
    updateRoutingTable();

    if(routingTable.size() == m_nNodes) {
        setPorts();
        // dumpRoutingTable(routingTable, "D:\\ComputerNetworks\\CN_CHomeworks_3\\data\\RoutingTable\\" + m_ip + ".json");
        Q_EMIT routingDone();
    }
}




void OSPF::updateRoutingTable() {

    // Dijkstra's algorithm
    QMap<QString, int> distances;
    QMap<QString, QString> predecessors;
    QSet<QString> visited;
    auto compare = [&distances](const QString &a, const QString &b) {
        return distances[a] > distances[b];
    };
    std::priority_queue<QString, QVector<QString>, decltype(compare)> pq(compare);

    for (const auto &router : topology.keys()) {
        distances[router] = std::numeric_limits<int>::max();
        for (const auto &neighbor : topology[router].keys()) {
            distances[neighbor] = std::numeric_limits<int>::max();
        }
    }
    distances[m_ip] = 0;
    pq.push(m_ip);

    while (!pq.empty()) {
        QString current = pq.top();
        pq.pop();
        if (visited.contains(current)) continue;
        visited.insert(current);

        for (const auto &neighbor : topology[current].keys()) {
            int newDist = distances[current] + topology[current][neighbor];
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                predecessors[neighbor] = current;
                pq.push(neighbor);
            }
        }
    }

    // Update routing table
    routingTable.clear();
    for (const auto &dest : distances.keys()) {
        if (dest == m_ip || distances[dest] == std::numeric_limits<int>::max()) continue;
        QString nextHop = dest;
        while (predecessors[nextHop] != m_ip) {
            nextHop = predecessors[nextHop];
        }
        routingTable[dest].ip = nextHop;
    }

}





RIP::RIP(QObject* parent)
    : RoutingProtocol(parent) {
}

bool RIP::isNeedToBeUpdate() const {
    return m_update;
}

void RIP::receiveRIP(const QMap<QString, RouteEntry>& routes) {
    m_update = false;


    for (auto it = routes.constBegin(); it != routes.constEnd(); ++it) {
        const QString& dest = it.key();
        const RouteEntry& receivedEntry = it.value();

        if (receivedEntry.metric >= INFINITY_METRIC) {
            continue;
        }

        if (!m_routingInfo.contains(dest) ||
           m_routingInfo[dest].metric > receivedEntry.metric + 1) {
            m_routingInfo[dest] = {receivedEntry.nextHop, receivedEntry.metric + 1};
            routingTable[dest].ip = receivedEntry.nextHop;
            setPorts();
            m_update = true;
        }
    }
}

QMap<QString, RIP::RouteEntry> RIP::getRoutingInfo() const {
    QMap<QString, RIP::RouteEntry> r;
    for (auto it = m_routingInfo.constBegin(); it != m_routingInfo.constEnd(); ++it) {
        r[it.key()] = {m_ip, it.value().metric};
    }

    return r;
}

void RIP::init() {
    m_routingInfo[m_ip] = {m_ip, 0};
}
