#ifndef AUTONOMOUSSYSTEM_H
#define AUTONOMOUSSYSTEM_H

#include <QObject>
#include <QVector>
#include "networknode.h"
#include "../applicationcontext.h"
#include "../PortBindingManager/PortBindingManager.h"
#include "../EventsCoordinator/EventsCoordinator.h"
#include "../simulator.h"

class AutonomousSystem : public QObject
{
    Q_OBJECT

public:
    explicit AutonomousSystem(const ApplicationContext::SimulationConfig&, const ApplicationContext::AutonomousSystemConfig&, EventsCoordinator *&, PortBindingManager *&, Simulator *&, QObject *parent = nullptr);
    Router* getRouterById(int id);
    int getId();
    QVector<int> getPcIds();
    void buildTopology(const ApplicationContext::AutonomousSystemConfig&, EventsCoordinator *&, PortBindingManager *&, const QString &);
    void buildPcs(const ApplicationContext::AutonomousSystemConfig&, EventsCoordinator *&, PortBindingManager *&);

Q_SIGNALS:

private:
    int m_id;
    QVector<Router*> m_routers;
    QVector<PC*> m_pcs;
    QVector<int> m_gateways;
    int m_dhcpServer;


};

#endif    // AUTONOMOUSSYSTEM_H
