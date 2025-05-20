#ifndef MACADDRESSGENERATOR_H
#define MACADDRESSGENERATOR_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QRandomGenerator>

class MACAddressGenerator : public QObject
{
    Q_OBJECT

public:
    explicit MACAddressGenerator(QObject *parent = nullptr);
    QString generateMacAddress();

private:
    QSet<QString> generatedMacAddresses;
    QString generateRandomMacAddress() const;

Q_SIGNALS:
};

#endif    // MACADDRESSGENERATOR_H
