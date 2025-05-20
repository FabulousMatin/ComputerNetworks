#ifndef ROUTINGPROTOCOL_H
#define ROUTINGPROTOCOL_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QSet>
#include <QVector>
#include <QPair>
#include <limits>
#include <queue>

class RoutingProtocol : public QObject
{
    Q_OBJECT

public:
    struct Route {
        QString ip;
        QString port;
    };
    explicit RoutingProtocol(QObject *parent = nullptr);
    void addNeighbor(const QString &neighborIp, int cost);
    QMap<QString, int> getNeighbors();
    void setIp(const QString &ip);
    void setIpToPort(const QMap<QString, QString> &);
    QString getNextHop(const QString &ip);
    QMap<QString, Route> getRoutingTable() const;

Q_SIGNALS:
    void routingDone();
public Q_SLOTS:

protected:
    QMap<QString, int> neighbors;
    QMap<QString, Route> routingTable;
    QString m_ip;
    int m_nNodes = 37;
    QMap<QString, QString> m_ipToPort;
    void setPorts();
};


class OSPF : public RoutingProtocol
{
    Q_OBJECT

public:
    explicit OSPF(QObject *parent = nullptr);
    void updateRoutingTable();
    void receiveLSA(const QString &from, const QMap<QString, int> &linkState);
    int getSeqNumber();
    void incSeqNumber();

Q_SIGNALS:

private:
    QMap<QString, QMap<QString, int>> topology;
    int seqNumber;
    QSet<QString> lsdb;
};



class RIP : public RoutingProtocol
{
    Q_OBJECT

public:
    struct RouteEntry {
        QString nextHop;
        int metric;
    };

    explicit RIP(QObject *parent = nullptr);
    void receiveRIP(const QMap<QString, RouteEntry>& routes);
    void init();
    QMap<QString, RouteEntry> getRoutingInfo() const;
    bool isNeedToBeUpdate() const;
Q_SIGNALS:


private:
    QMap<QString, RouteEntry> m_routingInfo;
    const int INFINITY_METRIC = 100;
    bool m_update = false;
};








#endif    // ROUTINGPROTOCOL_H
