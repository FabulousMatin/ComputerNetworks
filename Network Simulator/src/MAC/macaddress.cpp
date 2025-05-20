#include "macaddress.h"

MACAddress::MACAddress(QObject *parent) :
    QObject {parent}
{
    MACAddressGenerator generator;
    macAddress = generator.generateMacAddress();
}

QString MACAddress::toString() const {
    return macAddress;
}
