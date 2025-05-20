#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include "Globals/Globals.h"
#include <QMap>
#include <QString>
#include "Routing/routingprotocol.h"

class Simulator : public QObject
{
    Q_OBJECT

public:
    struct ForwardStats {
        int forwarded;
        int lost;
    };

    explicit Simulator(int nNodes, int nRouters, QObject *parent = nullptr);
    void begin();


private:
    NETWORK::State m_state = NETWORK::State::IDLE;
    int m_nNodes;
    int m_nRouters;

    int m_dchpDone = 0;
    int m_routingDone = 0;

    void nextState();
    void analyze();


    QVector<int> m_waitedCycles;
    QVector<int> m_totalCycles;
    QMap<QString, ForwardStats> m_routerStats;
    QMap<QString, QMap<QString, RoutingProtocol::Route>> m_routingTables;
    QVector<QVector<QString>> m_paths;


Q_SIGNALS:
    void changeState(NETWORK::State);

public Q_SLOTS:
    void dchpDone();
    void routingDone();
    void simulationDone();

    void cycleStatsReceived(int waited, int total);
    void forwardingStatsReceived(QString id, int forwarded, int lost);
    void routingTableReceived(QString id, QMap<QString, RoutingProtocol::Route> m);
    void pathReceived(QVector<QString> path);
};

#endif    // SIMULATOR_H
