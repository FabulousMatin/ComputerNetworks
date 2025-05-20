#ifndef EVENTSCOORDINATOR_H
#define EVENTSCOORDINATOR_H

#include <chrono>

#include <QObject>
#include <QThread>
#include "datagenerator.h"
#include <algorithm>
#include <chrono>
#include <QVector>
#include <random>
#include "../Globals/Globals.h"
#include "../Packet/packet.h"


typedef std::chrono::milliseconds Millis;

class EventsCoordinator : public QThread
{
    Q_OBJECT



    explicit EventsCoordinator(Millis cycleDuration, int cycles, QThread *parent = nullptr);

public:    // constructors
    ~EventsCoordinator() override = default;
    static EventsCoordinator *instance(Millis cycleDuration, int cycles, QThread *parent = nullptr);
    static void               release();
    void begin();

public:    // methods
    void createDataGenerator(QString dist, int totalPackets);
    void addPcIds(QVector<int> ids);
public:    // getter and setters
Q_SIGNALS:
    void simulationEnd();
    void sendBuffer(int id, const QVector<PacketPtr_t>& buffer);
    void clock();

public Q_SLOTS:
    void stateChanged(NETWORK::State);

private Q_SLOTS:

private:    // members
    inline static EventsCoordinator *m_self = nullptr;
    Millis m_cycleDuration;
    int m_cycles;
    int m_currCycle = 0;
    QVector<int> m_pcIds;
    NETWORK::State m_state = NETWORK::State::IDLE;

    DataGenerator *m_dataGenerator;
};

#endif    // EVENTSCOORDINATOR_H
