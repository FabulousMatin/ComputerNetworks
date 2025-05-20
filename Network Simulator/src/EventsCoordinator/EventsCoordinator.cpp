#include "EventsCoordinator.h"
#include <QDebug>
#include <QCoreApplication>

EventsCoordinator::EventsCoordinator(Millis cycleDuration, int cycles, QThread *parent) :
    QThread {parent}
{
    m_cycleDuration = cycleDuration;
    m_cycles = cycles;
}

void
EventsCoordinator::createDataGenerator(QString dist, int totalPackets) {
}

EventsCoordinator *
EventsCoordinator::instance(Millis cycleDuration, int cycles, QThread *parent)
{
    if(!m_self)
    {
        m_self = new EventsCoordinator(cycleDuration, cycles, parent);
    }

    return m_self;
}

void EventsCoordinator::stateChanged(NETWORK::State newState) {
    m_state = newState;
    qDebug() << "curr clock" << m_currCycle;

    if(m_state == NETWORK::State::DHCP) {
        begin();
    }
}

int lastNonZeroValueIndex(const QVector<int> &vec) {
    for (int i = vec.size() - 1; i >= 0; --i) {
        if (vec[i] != 0) {
            return i;
        }
    }
      return -1;
}

void EventsCoordinator::begin() {

    std::mt19937 rng;
    int tSimulate = 0;

    QList<QList<PacketPtr_t>> buffers = m_dataGenerator->getBuffers();
    for(int i = 0; i < m_pcIds.size(); i++) {
        Q_EMIT sendBuffer(m_pcIds[i], buffers[i]);
        qInfo() << "Packets generated for PC" << m_pcIds[i] << ":" << buffers[i].size();
    }

    m_currCycle = 0;
    while(tSimulate < m_cycles) {

        if(m_state == NETWORK::State::SIMULATION) {
            tSimulate++;
        }
        m_currCycle++;

        Q_EMIT clock();
        std::this_thread::sleep_for(this->m_cycleDuration);
        QCoreApplication::processEvents();
    }

    Q_EMIT simulationEnd();
}

void EventsCoordinator::addPcIds(QVector<int> ids) {
    m_pcIds = ids;
    m_pcIds.removeOne(24);
    m_dataGenerator = new DataGenerator(m_pcIds);
}

void
EventsCoordinator::release()
{
    if(m_self)
    {
        delete m_self;
        m_self = nullptr;
    }
}
