#ifndef TCPNEWRENO_H
#define TCPNEWRENO_H

#include <QSet>
#include <QObject>

class TCPNewRenoReceiver : public QObject
{
    Q_OBJECT

public:
    explicit TCPNewRenoReceiver(QObject *parent = nullptr);
    int expectedSeqNum = 0;
    int lastAck = -1;

Q_SIGNALS:
};


class TCPNewRenoSender  : public QObject
{
    Q_OBJECT

public:
    explicit TCPNewRenoSender(QObject *parent = nullptr);
    int cwnd = 1;  // Congestion Window
    int ssthresh = 1;  // Slow Start Threshold
    int nextSeqNum = 0;  // Next packet to send
    int lastAcked = -1;  // Last acknowledged packet
    int duplicateAcks = 0;  // Counter for duplicate ACKs
    bool inRecovery = false;
    QSet<int> ackeds;
    int head = 0;


Q_SIGNALS:
};


#endif    // TCPNEWRENO_H
