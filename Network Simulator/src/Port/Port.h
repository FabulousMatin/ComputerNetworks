#ifndef PORT_H
#define PORT_H

#include "../Packet/Packet.h"

#include <QObject>

class Port : public QObject
{
    Q_OBJECT

public:
    explicit Port(const QString &id, QObject *parent = nullptr);
    ~Port() override;
    void sendPacket(const PacketPtr_t &packet);

Q_SIGNALS:
    void packetSent(const PacketPtr_t &packet);
    void packetReceived(const PacketPtr_t &packet, const QString &portId);

public Q_SLOTS:

    void receivePacket(const PacketPtr_t &packet);
    QString getPortId();

private:
    QString m_id;
    uint64_t m_numberOfPacketsSent = 0;
};

typedef QSharedPointer<Port> PortPtr_t;

#endif    // PORT_H
