#ifndef BUFFER_H
#define BUFFER_H

#include <QObject>
#include <QQueue>
#include "../Packet/packet.h"

class Buffer : public QObject
{
    Q_OBJECT

public:
    explicit Buffer(QObject *parent = nullptr);
    int push(const PacketPtr_t &packet);
    QQueue<PacketPtr_t> m_queue;
    PacketPtr_t pop();
    bool isFull();
    int size();
    void clear();
    bool isEmpty();

    void waitOneCycle();
    void passOneCycle();
    void setMaxSize(int maxSize);


private:

    int m_maxSize = 1000;
};

#endif    // BUFFER_H
