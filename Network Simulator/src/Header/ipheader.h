#ifndef IPHEADER_H
#define IPHEADER_H

#include <QObject>
#include "../IP/IP.h"

class IPHeader : public QObject
{
    Q_OBJECT

public:
    explicit IPHeader(QObject *parent = nullptr);

private:

Q_SIGNALS:
};

class IPv4Header : public IPHeader
{
public:
    explicit IPv4Header(QObject *parent = nullptr);

private:
    uint8_t m_versionHeaderLength;
    uint8_t m_typeOfService;
    uint16_t m_totalLength;
    uint16_t m_identification;
    uint16_t m_flagsFragmentOffset;
    uint8_t m_ttl;
    uint8_t m_protocol;
    uint16_t m_headerChecksum;
    IPv4Ptr_t m_sourceIp;
    IPv4Ptr_t m_destIp;
};

class IPv6Header : public IPHeader
{
public:
    explicit IPv6Header(QObject *parent = nullptr);

private:
    uint32_t m_versionTrafficClassFlowLabel;
    uint16_t m_payloadLength;
    uint8_t m_nextHeader;
    uint8_t m_hopLimit;
    IPv6Ptr_t m_sourceIP;
    IPv6Ptr_t m_destIP;
};

#endif    // IPHEADER_H
