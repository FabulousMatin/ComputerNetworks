#include "macaddressgenerator.h"

MACAddressGenerator::MACAddressGenerator(QObject *parent) :
    QObject {parent}
{}


QString MACAddressGenerator::generateMacAddress() {
    QString mac;
    do {
        mac = generateRandomMacAddress();
    } while (generatedMacAddresses.contains(mac)); // Ensure uniqueness

    generatedMacAddresses.insert(mac);
    return mac;
}

QString MACAddressGenerator::generateRandomMacAddress() const {
    QString macAddress;
    for (int i = 0; i < 6; ++i) {
        QString octet = QString("%1").arg(QRandomGenerator::global()->bounded(0, 256), 2, 16, QChar('0')).toUpper();
        macAddress.append(octet);
        if (i < 5)
            macAddress.append(":");
    }
    return macAddress;
}
