#include "simulator.h"
#include <QDebug>
#include <QTextStream>
#include <QCoreApplication>

Simulator::Simulator(int nNodes, int nRouters, QObject *parent) :
    QObject {parent},
    m_nNodes(nNodes),
    m_nRouters(nRouters)
{}


void Simulator::dchpDone() {
    m_dchpDone++;
    // qDebug() << m_dchpDone;
    if(m_dchpDone == m_nNodes) {
        m_state = NETWORK::State::ROUTING;
        nextState();
    }
}

void Simulator::routingDone() {
    m_routingDone++;

    if(m_routingDone == m_nRouters) {
        m_state = NETWORK::State::SIMULATION;
        nextState();
    }
}

void Simulator::simulationDone() {
    m_state = NETWORK::State::ANALYZE;
    nextState();
    analyze();
}

void Simulator::cycleStatsReceived(int waited, int total) {
    m_waitedCycles.append(waited);
    m_totalCycles.append(total);
}

void Simulator::forwardingStatsReceived(QString id, int forwarded, int lost) {
    m_routerStats[id] = {forwarded, lost};
}

void Simulator::routingTableReceived(QString id, QMap<QString, RoutingProtocol::Route> m) {
    m_routingTables[id] = m;
}

void Simulator::pathReceived(QVector<QString> path) {
    m_paths.append(path);
}

void Simulator::analyze() {
    QTextStream inputStream(stdin);  // For reading user input
    QTextStream outputStream(stdout); // For writing to terminal


    QTextStream in(stdin);
    QTextStream out(stdout);

    while (true) {
        QCoreApplication::processEvents();
        out << "Enter command (stats/router/waited/total/exit/table/paths/forward): ";
        out.flush();
        QString command = in.readLine();

        if (command == "exit") {
            break;
        } else if (command == "forward") {
            QVector<QPair<QString, int>> stats;
            for (auto it = m_routerStats.cbegin(); it != m_routerStats.cend(); ++it) {
                stats.append(qMakePair(it.key(), it.value().forwarded));
            }

            std::sort(stats.begin(), stats.end(), [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                return a.second > b.second; // Descending order
            });

            for (const auto& stat : stats) {
                out << stat.first << "->" << stat.second << "\n";
            }
        } else if(command == "stats") {
            int lost = 0;
            for (auto it = m_routerStats.begin(); it != m_routerStats.end(); ++it) {
                lost += it->lost;
            }
            int forwarded = m_paths.size();
            float ratio = float(lost) / float(lost + forwarded);
            out << "Forwaded=" << forwarded << ", lost=" << lost << ", packet loss percent=" << ratio << "\n";
        } else if (command == "waited") {
            int min = *std::min_element(m_waitedCycles.begin(), m_waitedCycles.end());
            int max = *std::max_element(m_waitedCycles.begin(), m_waitedCycles.end());
            double mean = std::accumulate(m_waitedCycles.begin(), m_waitedCycles.end(), 0.0) / m_waitedCycles.size();

            out << "Waited Cycles: Min=" << min << ", Max=" << max << ", Mean=" << mean << "\n";
        } else if (command == "total") {
            int min = *std::min_element(m_totalCycles.begin(), m_totalCycles.end());
            int max = *std::max_element(m_totalCycles.begin(), m_totalCycles.end());
            double mean = std::accumulate(m_totalCycles.begin(), m_totalCycles.end(), 0.0) / m_totalCycles.size();

            out << "Total Cycles: Min=" << min << ", Max=" << max << ", Mean=" << mean << "\n";
        } else if (command == "stats") {
            out << "Enter router name: ";
            out.flush();
            QString router = in.readLine();

            if (m_routerStats.contains(router)) {
                ForwardStats stats = m_routerStats[router];
                out << "Router: " << router << "\nForwarded: " << stats.forwarded << "\nLost: " << stats.lost << "\n";
            } else {
                out << "Router not found.\n";
            }
        } else if (command == "table") {
            out << "Enter router name: ";
            out.flush();
            QString router = in.readLine();

            if (m_routingTables.contains(router)) {
                out << "Routing Table for " << router << ":\n";
                for (auto it = m_routingTables[router].begin(); it != m_routingTables[router].end(); ++it) {
                    out << it.key() << " -> " << it.value().ip << ", " << it.value().port << "\n";
                }
            } else {
                out << "Router not found.\n";
            }

        } else if (command == "paths") {
            QVector<int> pathSizes;
            for (const auto &path : m_paths) {
                pathSizes.append(path.size());
            }

            int min = *std::min_element(pathSizes.begin(), pathSizes.end());
            int max = *std::max_element(pathSizes.begin(), pathSizes.end());
            double mean = std::accumulate(pathSizes.begin(), pathSizes.end(), 0.0) / pathSizes.size();

            out << "Path Sizes: Min=" << min << ", Max=" << max << ", Mean=" << mean << "\n";
        } else {
            out << "Unknown command.\n";
        }

    }


    m_state = NETWORK::State::END;
    nextState();
    QCoreApplication::quit();
}

void Simulator::begin() {
    m_state = NETWORK::State::DHCP;
    nextState();
}


void Simulator::nextState() {
    qDebug() << "state changed to" << int(m_state);
    Q_EMIT changeState(m_state);
}
