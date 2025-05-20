#ifndef MACADDRESS_H
#define MACADDRESS_H

#include <QObject>
#include <QString>
#include "macaddressgenerator.h"

class MACAddress : public QObject
{
    Q_OBJECT

public:
    explicit MACAddress(QObject *parent = nullptr);
    QString toString() const;

private:
    QString macAddress;
Q_SIGNALS:
};

#endif    // MACADDRESS_H
