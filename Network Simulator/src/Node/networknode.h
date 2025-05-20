#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <QThread>
#include "../IP/IP.h"
#include "../Routing/routingprotocol.h"
#include "../Buffer/buffer.h"
#include "../Port/Port.h"
#include <QVector>
#include <QMap>
#include "../Packet/packet.h"
#include "../Globals/Globals.h"
#include <QSet>
#include "../TCP/tcpnewreno.h"

class NetworkNode : public QThread
{
    Q_OBJECT

public:
    explicit NetworkNode(int id, int asId, QThread *parent = nullptr);
    int getId();
protected:
    enum DHCPState {
        NO_IP,
        DISCOVERY,
        REQUEST,
        ACK,
        IP_DONE
    };
    enum OSPFState {
        OspfOff,
        Hello,
        NEIGHBOR,
        LSA,
        LSASent
    };
    enum RIPState {
        RipOff,
        INIT,
        UPDATE,
        RipDone
    };

    int m_id;
    int m_asId;
    IPv4Ptr_t m_ipv4;
    IPv6Ptr_t m_ipv6;
    NETWORK::State m_networkState = NETWORK::State::IDLE;
    int m_dhcpState = DHCPState::NO_IP;

Q_SIGNALS:
    void ipDone();

public Q_SLOTS:
    void stateChanged(NETWORK::State);
};

class PC : public NetworkNode
{
    Q_OBJECT
public:
    explicit PC(int id, int asId, QThread *parent = nullptr);
    PortPtr_t m_port;

Q_SIGNALS:
    void sendCycleStats(int waited, int total);
    void sendPath(QVector<QString> path);

public Q_SLOTS:
    void nextClock();
    void getBuffer(int id, const QVector<PacketPtr_t>& packets);
    void receivePacket(const PacketPtr_t &packet, const QString &portId);

private:
    int m_nRecievedPackets = 0;
    QVector<PacketPtr_t> m_senderBuffer;
    QVector<QByteArray> m_receiverBuffer;

    QQueue<PacketPtr_t> m_receiverAckQueue;

    QMap<int, TCPNewRenoReceiver*> m_tcpReceiveInfo;
    TCPNewRenoSender* m_tcpSenderInfo;

    void processTcpSender(const PacketPtr_t &packet);
    void processTcpReceiver(const PacketPtr_t &packet);

};

class Router : public NetworkNode
{

    Q_OBJECT

public:
    explicit Router(int id, bool isDhcp, bool isBroken, int asId, QString routing, QThread *parent = nullptr);
    QMap<QString, PortPtr_t> m_ports;
    RoutingProtocol *getRoutingProtocol();
Q_SIGNALS:
    void routingDone();
    void sendForwardingStats(QString id, int forwarded, int lost);
    void sendRoutingTable(QString id, QMap<QString, RoutingProtocol::Route> routingTable);

public Q_SLOTS:
    void nextClock();
    void receivePacket(const PacketPtr_t &packet, const QString &portId);
    void stateChanged(NETWORK::State);

private:
    RoutingProtocol *m_routingProtocol;
    Buffer *m_buffer;
    bool m_isDhcp;
    bool m_isBroken = false;
    int m_ospfState = OSPFState::OspfOff;
    int m_ripState = RIPState::RipOff;
    int m_ripNotUpdated = 0;
    int m_osfpNeighborDone = 0;
    void broadcast(const PacketPtr_t &packet);
    void processPacket(const PacketPtr_t &packet);
    void processDhcpPacket(const PacketPtr_t &packet);
    void processOspfPacket(const PacketPtr_t &packet);
    void processRipPacket(const PacketPtr_t &packet);
    bool isDhcpPacket(NETWORK::PacketControlType);
    bool isOspfPacket(NETWORK::PacketControlType);
    QMap<QString, QString> m_ipToPort;
    void forwardPacket(const PacketPtr_t &packet);


    int m_forwarded = 0;
    int m_lostPackets = 0;
};

#endif    // NETWORKNODE_H
