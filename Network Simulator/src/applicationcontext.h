#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QVariantMap>
#include <vector>


class ApplicationContext
{
public:
    static int getTTL();
    static int getRouterBufferSize();

    struct AutonomousSystemConfig {
        struct GatewayConfig {
            int node;
            std::vector<int> users;
        };
        struct ConnectionConfig {
            struct GatewayPairConfig {
                int gateway;
                int connectTo;
            };
            int id;
            std::vector<GatewayPairConfig> gatewayPairs;
        };
        int id;
        QString topologyType;
        int firstNode;
        int nodeCount;
        std::vector<int> asGateways;
        std::vector<int> userGateways;
        int dhcpServer;
        QSet<int> brokenRouters;
        std::vector<GatewayConfig> gateways;
        std::vector<ConnectionConfig> connections;
    };

    struct SimulationConfig {
        int simulationDuration;
        int cycleDuration;
        int ttl;
        int packetsPerSimulation;
        QString statisticalDistribution;
        int routerBufferSize;
        int routerPortCount;
        QString routingProtocol;
        QString routingTableUpdateInterval;
        bool routingPerPort;
        int routingTableSize;
        int routingPacketsPerPortCycle;
        std::vector<AutonomousSystemConfig> autonomousSystems;
    };


    ApplicationContext();
    SimulationConfig readConfigFile(const QString &filePath);
    SimulationConfig config;
};

#endif    // APPLICATIONCONTEXT_H
