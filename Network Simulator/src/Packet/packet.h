#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include "../Header/bgpheader.h"
#include "../Header/datalinkheader.h"
#include "../Header/ipheader.h"
#include "../Header/tcpheader.h"
#include <QSharedPointer>
#include "../Globals/Globals.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QRandomGenerator>
#include "../Routing/routingprotocol.h"


class Packet : public QObject {
    Q_OBJECT

public:
    enum Type { Data, Control };

    explicit Packet(Type type, IPHeader *ipHeader, BGPHeader *bgpHeader, TCPHeader *tcpHeader,
                    DataLinkHeader *dataLinkHeader, const QString &payload,
                    int sequenceNumber, QObject *parent = nullptr);

    explicit Packet();
    virtual ~Packet() = default;
    explicit Packet(const Packet &);

    QString toString() const;

    void addPathHop(const QString &hop);
    void incrementWaitingCycles();
    void incrementTotalCycles();
    void finalizeCycles(int totalCycles);
    void decTtl();
    void addPath(QString p);

    // Setters
    void setType(Type type);
    void setControlType(NETWORK::PacketControlType controlType);
    void setIPHeader(IPHeader *ipHeader);
    void setBGPHeader(BGPHeader *bgpHeader);
    void setTCPHeader(TCPHeader *tcpHeader);
    void setDataLinkHeader(DataLinkHeader *dataLinkHeader);
    void setPayload(const QString &payload);
    void setSequenceNumber(int sequenceNumber);
    void setTcpDataSeq(int seq);
    void setWaitingCycles(int waitingCycles);
    void setTotalCycles(int totalCycles);
    void setTtl(int);
    void setPortIdReceived(const QString &id);
    void setSourceIp(const QString &ip);
    void setDestIp(const QString &ip);

    // Getters
    Type getType() const;
    NETWORK::PacketControlType getControlType() const;
    IPHeader *getIPHeader() const;
    BGPHeader *getBGPHeader() const;
    TCPHeader *getTCPHeader() const;
    DataLinkHeader *getDataLinkHeader() const;
    QString getPayload() const;
    int getSequenceNumber() const;
    int getTcpDataSeq() const;
    int getWaitingCycles() const;
    int getTotalCycles() const;
    QList<QString> getPath() const;
    int getTtl() const;
    QString getPortIdReceived() const;
    QString getSourceIp() const;
    QString getDestIp() const;

    static QSharedPointer<Packet> createDiscoveryPacket(const int &asId, const QString &id) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Control);
        newPacket->setControlType(NETWORK::PacketControlType::DHCPDiscovery);

        QJsonObject res;
        res["ID"] = id;
        res["AS"] = asId;
        QJsonDocument resDoc(res);
        QString response = resDoc.toJson();
        newPacket->setPayload(response);

        return newPacket;
    }

    static QSharedPointer<Packet> createOfferPacket(const int &asId, const QString &id) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Control);
        newPacket->setControlType(NETWORK::PacketControlType::DHCPOffer);

        QJsonObject res;
        res["ID"] = id;
        res["AS"] = asId;
        res["IP"] = "192.168." + QString::number(asId) + "." + id;
        QJsonDocument resDoc(res);
        QString response = resDoc.toJson();
        newPacket->setPayload(response);

        return newPacket;
    }

    static QSharedPointer<Packet> createReqPacket(const int &asId, const QString &id) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Control);
        newPacket->setControlType(NETWORK::PacketControlType::DHCPRequest);

        QJsonObject res;
        res["ID"] = id;
        res["AS"] = asId;
        QJsonDocument resDoc(res);
        QString response = resDoc.toJson();
        newPacket->setPayload(response);

        return newPacket;
    }

    static QSharedPointer<Packet> createAckPacket(const int &asId, const QString &id) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Control);
        newPacket->setControlType(NETWORK::PacketControlType::DHCPAcknowledge);

        QJsonObject res;
        res["ID"] = id;
        res["AS"] = asId;
        QJsonDocument resDoc(res);
        QString response = resDoc.toJson();
        newPacket->setPayload(response);

        return newPacket;
    }

    static QSharedPointer<Packet> createDataPacket(const QByteArray &data) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Data);

        newPacket->setPayload(QString::fromLocal8Bit(data));
        newPacket->setDestIp("192.168.1.24");

        return newPacket;
    }

    static QSharedPointer<Packet> createTcpAckPacket(const QString ack, const QString &destIp) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Data);
        // newPacket->setControlType(NETWORK::PacketControlType::TCPAck);

        newPacket->setPayload(ack);
        newPacket->setDestIp(destIp);

        return newPacket;
    }


    static QSharedPointer<Packet> createOspfHelloPacket(const QString &ip) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Control);
        newPacket->setControlType(NETWORK::PacketControlType::OSPFHello);

        QJsonObject res;
        res["IP"] = ip;
        QJsonDocument resDoc(res);
        QString response = resDoc.toJson();
        newPacket->setPayload(response);

        return newPacket;
    }

    static QSharedPointer<Packet> createLSAPacket(const QString &from, const QMap<QString, int> &linkState) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Control);
        newPacket->setControlType(NETWORK::PacketControlType::OSPFLsa);

        QJsonObject LSObject;
        for (auto it = linkState.begin(); it != linkState.end(); ++it) {
            LSObject.insert(it.key(), it.value());
        }

        QJsonObject res;
        res["FROM"] = from;
        res["LINKSTATE"] = LSObject;

        QJsonDocument resDoc(res);
        QString response = resDoc.toJson();
        newPacket->setPayload(response);
        return newPacket;
    }

    static QSharedPointer<Packet> createRIPPacket(const QMap<QString, RIP::RouteEntry> &rip, const QString &ip) {
        QSharedPointer<Packet> newPacket = QSharedPointer<Packet>::create();
        newPacket->setType(Type::Control);
        newPacket->setControlType(NETWORK::PacketControlType::RIP);
        newPacket->setSourceIp(ip);


        QJsonArray routesArray;
        for (auto it = rip.begin(); it != rip.end(); ++it) {
            QJsonObject RIPObject;
            RIPObject["dest"] = it.key();
            RIPObject["nextHop"] = it.value().nextHop;
            RIPObject["metric"] = it.value().metric;

            routesArray.append(RIPObject);

        }

        QJsonObject res;
        res["RIP"] = routesArray;

        QJsonDocument resDoc(res);
        QString response = resDoc.toJson();
        newPacket->setPayload(response);
        return newPacket;
    }

protected:
    Type m_type;
    NETWORK::PacketControlType m_ControlType;

    IPHeader *m_IPHeader;
    BGPHeader *m_BGPHeader;
    TCPHeader *m_TCPHeader;
    DataLinkHeader *m_dataLinkHeader;
    QString m_payload;
    int m_ttl = 10; // ApplicationContext::getTTL();
    int m_sequenceNumber;
    int m_tcpDataSeq;
    int m_waitingCycles = 0;
    int m_totalCycles = 0;
    QList<QString> m_path;
    QString m_portIdReceived = "";
    QString m_sourceIp;
    QString m_destIp;

Q_SIGNALS:
};

typedef QSharedPointer<Packet> PacketPtr_t;

#endif    // PACKET_H
