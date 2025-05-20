#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include "autonomoussystem.h"
#include "../applicationcontext.h"
#include "../simulator.h"

class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(const ApplicationContext::SimulationConfig, EventsCoordinator *&, Simulator *&, QObject *parent = nullptr);

Q_SIGNALS:

private:
    QVector<AutonomousSystem*> m_autonomousSystems;

};

#endif    // NETWORK_H
