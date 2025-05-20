#ifndef DATALINKHEADER_H
#define DATALINKHEADER_H

#include <QObject>
#include "../MAC/macaddress.h"

class DataLinkHeader : public QObject
{
    Q_OBJECT

public:
    explicit DataLinkHeader(MACAddress *sourceMACAddress, MACAddress *destinationMACAddress, QObject *parent = nullptr);

private:
    MACAddress *m_sourceMACAddress;
    MACAddress *m_destinationMACAddress;

};

#endif    // DATALINKHEADER_H
