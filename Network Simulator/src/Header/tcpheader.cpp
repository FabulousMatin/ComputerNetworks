#include "tcpheader.h"

TCPHeader::TCPHeader(QObject *parent, uint16_t sourcePort, uint16_t destPort, uint32_t sequenceNumber, 
                     uint32_t acknowledgmentNumber, uint8_t dataOffset, uint8_t flags, 
                     uint16_t windowSize, uint16_t checksum, uint16_t urgentPointer) :
    QObject {parent},
    m_sourcePort(sourcePort), 
    m_destPort(destPort), 
    m_sequenceNumber(sequenceNumber),
    m_acknowledgmentNumber(acknowledgmentNumber), 
    m_dataOffset(dataOffset), 
    m_flags(flags),
    m_windowSize(windowSize), 
    m_checksum(checksum), 
    m_urgentPointer(urgentPointer) 
{}

