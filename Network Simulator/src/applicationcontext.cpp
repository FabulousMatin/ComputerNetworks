#include "applicationcontext.h"

ApplicationContext::ApplicationContext() {
    config = readConfigFile("D:\\ComputerNetworks\\CN_CHomeworks_3\\assets\\config.json");

}
int ApplicationContext::getTTL() {
    QFile file("D:\\ComputerNetworks\\CN_CHomeworks_3\\assets\\config.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file.";
        return 10;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();
    return root["TTL"].toInt();
}

int ApplicationContext::getRouterBufferSize() {
    // QFile file("D:\\ComputerNetworks\\CN_CHomeworks_3\\assets\\config.json");
    // if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //     qDebug() << "Unable to open file.";
    //     return 6;
    // }
    // QByteArray data = file.readAll();
    // file.close();

    // QJsonDocument doc = QJsonDocument::fromJson(data);
    // QJsonObject root = doc.object();
    // return root["router_buffer_size"].toInt();

    return 15;
}

ApplicationContext::SimulationConfig
ApplicationContext::readConfigFile(const QString &filePath) {
    SimulationConfig config;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file.";
        return config;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON structure.";
        return config;
    }

    QJsonObject root = doc.object();

    config.simulationDuration = root["simulation_duration"].toInt();
    config.cycleDuration = root["cycle_duration"].toInt();
    config.ttl = root["TTL"].toInt();
    config.packetsPerSimulation = root["packets_per_simulation"].toInt();
    config.statisticalDistribution = root["statistical_distribution"].toString();
    config.routerBufferSize = root["router_buffer_size"].toInt();
    config.routerPortCount = root["router_port_count"].toInt();
    config.routingProtocol = root["routing_protocol"].toString();
    config.routingTableUpdateInterval = root["routing_table_update_interval"].toString();
    config.routingPerPort = root["routing_per_port"].toBool();
    config.routingTableSize = root["routing_table_size"].toInt();
    config.routingPacketsPerPortCycle = root["routing_packets_per_port_cycle"].toInt();

    QJsonArray autonomousSystems = root["Autonomous_systems"].toArray();
    for (const QJsonValue &asValue : autonomousSystems) {
        QJsonObject asObject = asValue.toObject();

        AutonomousSystemConfig as;
        as.id = asObject["id"].toInt();
        as.topologyType = asObject["topology_type"].toString();
        as.firstNode = asObject["first_node"].toInt();
        as.nodeCount = asObject["node_count"].toInt();

        for (const QJsonValue &gateway : asObject["as_gateways"].toArray()) {
            as.asGateways.push_back(gateway.toInt());
        }

        for (const QJsonValue &userGateway : asObject["user_gateways"].toArray()) {
            as.userGateways.push_back(userGateway.toInt());
        }

        as.dhcpServer = asObject["dhcp_server"].toInt();

        for (const QJsonValue &brokenRouter : asObject["broken_routers"].toArray()) {
            as.brokenRouters.insert(brokenRouter.toInt());
        }

        QJsonArray gateways = asObject["gateways"].toArray();
        for (const QJsonValue &gatewayValue : gateways) {
            QJsonObject gatewayObject = gatewayValue.toObject();
            AutonomousSystemConfig::GatewayConfig gateway;
            gateway.node = gatewayObject["node"].toInt();
            for (const QJsonValue &user : gatewayObject["users"].toArray()) {
                gateway.users.push_back(user.toInt());
            }
            as.gateways.push_back(gateway);
        }

        QJsonArray connections = asObject["connect_to_as"].toArray();
        for (const QJsonValue &connectionValue : connections) {
            QJsonObject connectionObject = connectionValue.toObject();
            AutonomousSystemConfig::ConnectionConfig connection;
            connection.id = connectionObject["id"].toInt();

            QJsonArray gatewayPairs = connectionObject["gateway_pairs"].toArray();
            for (const QJsonValue &pairValue : gatewayPairs) {
                QJsonObject pairObject = pairValue.toObject();
                AutonomousSystemConfig::ConnectionConfig::GatewayPairConfig pair;
                pair.gateway = pairObject["gateway"].toInt();
                pair.connectTo = pairObject["connect_to"].toInt();
                connection.gatewayPairs.push_back(pair);
            }
            as.connections.push_back(connection);
        }

        config.autonomousSystems.push_back(as);
    }

    return config;
}

