#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <QObject>

class TCPHeader : public QObject
{
    Q_OBJECT

public:
    explicit TCPHeader(QObject *parent,
              uint16_t sourcePort, uint16_t destPort, uint32_t sequenceNumber, 
              uint32_t acknowledgmentNumber, uint8_t dataOffset, uint8_t flags, 
              uint16_t windowSize, uint16_t checksum, uint16_t urgentPointer);

    std::string toString() const;

private:
    uint16_t m_sourcePort;
    uint16_t m_destPort;
    uint32_t m_sequenceNumber;
    uint32_t m_acknowledgmentNumber;
    uint8_t m_dataOffset;
    uint8_t m_flags;      // Flags: URG, ACK, PSH, RST, SYN, FIN
    uint16_t m_windowSize;
    uint16_t m_checksum;
    uint16_t m_urgentPointer;
};

#endif    // TCPHEADER_H
