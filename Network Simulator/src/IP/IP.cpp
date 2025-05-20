#include "IP.h"
#include <QHostAddress>
#include <QRegularExpression>

IP::IP(QObject *parent) : QObject(parent) {
}

IPv4::IPv4(QObject *parent)
    : IP(parent), m_ipValue(0)
{
}

IPv4::IPv4(const QString &ipString, QObject *parent)
    : IP(parent)
{
    QHostAddress address(ipString);
    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        m_ipValue = address.toIPv4Address();
    } else {
        m_ipValue = 0;
    }
}

IPv4::IPv4(uint64_t ipValue, QObject *parent)
    : IP(parent), m_ipValue(ipValue) {}

IPv4::~IPv4() {}

QString IPv4::toString() const {

    uint8_t octets[4];
    octets[0] = (m_ipValue >> 24) & 0xFF;
    octets[1] = (m_ipValue >> 16) & 0xFF;
    octets[2] = (m_ipValue >> 8) & 0xFF;
    octets[3] = m_ipValue & 0xFF;

    return QString("%1.%2.%3.%4").arg(octets[0]).arg(octets[1]).arg(octets[2]).arg(octets[3]);
}

uint64_t IPv4::toNumeric() const {
    return m_ipValue;
}

IPv6::IPv6(QObject *parent)
    : IP(parent), m_ipValue("0:0:0:0:0:0:0:0")
{}

IPv6::IPv6(const QString &ipString, QObject *parent)
    : IP(parent)
{
    QHostAddress address(ipString);
    if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        m_ipValue = ipString;
    } else {
        m_ipValue.clear();
    }
}

IPv6::~IPv6() {}

QString IPv6::toString() const {
    return m_ipValue;
}

typedef QSharedPointer<IPv4> IPv4Ptr;
typedef QSharedPointer<IPv6> IPv6Ptr;
