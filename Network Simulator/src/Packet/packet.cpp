#include "packet.h"

Packet::Packet(Type type, IPHeader *ipHeader, BGPHeader *bgpHeader, TCPHeader *tcpHeader,
               DataLinkHeader *dataLinkHeader, const QString& payload,
               int sequenceNumber, QObject *parent)
    :
    QObject {parent},
    m_type(type),
    m_IPHeader(ipHeader),
    m_BGPHeader(bgpHeader),
    m_TCPHeader(tcpHeader),
    m_dataLinkHeader(dataLinkHeader),
    m_payload(payload),
    m_sequenceNumber(sequenceNumber),
    m_tcpDataSeq(-1),
    m_waitingCycles(0),
    m_totalCycles(0) {}


Packet::Packet()
    : QObject(nullptr),
    m_type(Type::Data),
    m_IPHeader(nullptr),
    m_BGPHeader(nullptr),
    m_TCPHeader(nullptr),
    m_dataLinkHeader(nullptr),
    m_sequenceNumber(0),
    m_waitingCycles(0),
    m_totalCycles(0) {}


Packet::Packet(const Packet &other)
    : QObject(other.parent()), // Copy the QObject parent
    m_type(other.m_type),
    m_ControlType(other.m_ControlType),
    m_IPHeader(nullptr),
    m_BGPHeader(nullptr),
    m_TCPHeader(nullptr),
    m_dataLinkHeader(nullptr),
    m_payload(other.m_payload),
    m_ttl(other.m_ttl),
    m_sequenceNumber(other.m_sequenceNumber),
    m_waitingCycles(other.m_waitingCycles),
    m_totalCycles(other.m_totalCycles),
    m_path(other.m_path) {}


void Packet::addPath(QString p) {
    m_path.append(p);
}

void Packet::decTtl() {
    m_ttl--;
}

void Packet::addPathHop(const QString& hop) {
    m_path.append(hop);
}

void Packet::incrementWaitingCycles() {
    ++m_waitingCycles;
}


void Packet::incrementTotalCycles() {
    ++m_totalCycles;
}


void Packet::finalizeCycles(int totalCycles) {
    this->m_totalCycles = totalCycles;
}

// Setters
void Packet::setType(Type type) { m_type = type; }
void Packet::setControlType(NETWORK::PacketControlType controlType) { m_ControlType = controlType; }
void Packet::setIPHeader(IPHeader *ipHeader) { m_IPHeader = ipHeader; }
void Packet::setBGPHeader(BGPHeader *bgpHeader) { m_BGPHeader = bgpHeader; }
void Packet::setTCPHeader(TCPHeader *tcpHeader) { m_TCPHeader = tcpHeader; }
void Packet::setDataLinkHeader(DataLinkHeader *dataLinkHeader) { m_dataLinkHeader = dataLinkHeader; }
void Packet::setPayload(const QString &payload) { m_payload = payload; }
void Packet::setSequenceNumber(int sequenceNumber) { m_sequenceNumber = sequenceNumber; }
void Packet::setTcpDataSeq(int seq) { m_tcpDataSeq = seq; }
void Packet::setWaitingCycles(int waitingCycles) { m_waitingCycles = waitingCycles; }
void Packet::setTotalCycles(int totalCycles) { m_totalCycles = totalCycles; }
void Packet::setTtl(int ttl) {m_ttl = ttl;}
void Packet::setPortIdReceived(const QString &id) {m_portIdReceived = id;}
void Packet::setSourceIp(const QString &ip) {m_sourceIp = ip;}
void Packet::setDestIp(const QString &ip) {m_destIp = ip;}

// Getters
Packet::Type Packet::getType() const { return m_type; }
NETWORK::PacketControlType Packet::getControlType() const { return m_ControlType; }
IPHeader *Packet::getIPHeader() const { return m_IPHeader; }
BGPHeader *Packet::getBGPHeader() const { return m_BGPHeader; }
TCPHeader *Packet::getTCPHeader() const { return m_TCPHeader; }
DataLinkHeader *Packet::getDataLinkHeader() const { return m_dataLinkHeader; }
QString Packet::getPayload() const { return m_payload; }
int Packet::getSequenceNumber() const { return m_sequenceNumber; }
int Packet::getTcpDataSeq() const { return m_tcpDataSeq; }
int Packet::getWaitingCycles() const { return m_waitingCycles; }
int Packet::getTotalCycles() const { return m_totalCycles; }
QList<QString> Packet::getPath() const { return m_path; }
int Packet::getTtl() const {return m_ttl;}
QString Packet::getPortIdReceived() const {return m_portIdReceived;}
QString Packet::getSourceIp() const {return m_sourceIp;};
QString Packet::getDestIp() const {return m_destIp;};

