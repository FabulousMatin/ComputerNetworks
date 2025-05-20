#include "datalinkheader.h"

DataLinkHeader::DataLinkHeader(MACAddress *sourceMACAddress, MACAddress *destinationMACAddress, QObject *parent)

    : QObject {parent},
    m_sourceMACAddress(sourceMACAddress),
    m_destinationMACAddress(destinationMACAddress)
{}
