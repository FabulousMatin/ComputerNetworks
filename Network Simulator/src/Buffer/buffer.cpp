#include "buffer.h"
#include <QDebug>
#include <algorithm>

Buffer::Buffer(QObject *parent) :
    QObject {parent}
{}

void Buffer::setMaxSize(int m) {
    m_maxSize = m;
}

int Buffer::push(const PacketPtr_t &packet) {

    if (packet->getType() == Packet::Type::Control) {
        // Case 1: If the queue is full and contains only control packets, drop the new control packet.
        if (isFull() && std::all_of(m_queue.begin(), m_queue.end(), [](PacketPtr_t p) {
               return p->getType() == Packet::Control;
           })) {
            qDebug() << "Buffer is full with control packets. Control packet dropped.";
            return 0;
        }
        // Case 2: If no data packets in the queue, push the control packet to the last.
        if (std::none_of(m_queue.begin(), m_queue.end(), [](PacketPtr_t p) {
               return p->getType() == Packet::Type::Data;
           })) {
            m_queue.enqueue(packet);
            return 1;
        }
        else {
            // Case 3: Insert control packet before the first data packet or at the end of control packets
            auto it = std::find_if(m_queue.begin(), m_queue.end(), [](PacketPtr_t p) {
                return p->getType() == Packet::Type::Data;
            });
            m_queue.insert(it, packet);

            return 1;
        }

    } else if (packet->getType() == Packet::Type::Data) {
        if (!isFull()) {
            m_queue.enqueue(packet);

            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}

void Buffer::clear() {
    m_queue.clear();
}

void Buffer::waitOneCycle() {
    for(const PacketPtr_t &p : m_queue) {
        p->incrementWaitingCycles();
    }
}

void Buffer::passOneCycle() {
    for(const PacketPtr_t &p : m_queue) {
        p->incrementTotalCycles();
    }
}

PacketPtr_t Buffer::pop() {
    if (m_queue.isEmpty()) {
        return nullptr;
    }
    return m_queue.dequeue();
}

int Buffer::size() {
    return m_queue.size();
}


bool Buffer::isFull() {
    return m_queue.size() == m_maxSize;
}

bool Buffer::isEmpty() {
    return m_queue.isEmpty();
}
