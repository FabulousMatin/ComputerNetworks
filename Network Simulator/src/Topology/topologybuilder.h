#ifndef TOPOLOGYBUILDER_H
#define TOPOLOGYBUILDER_H

#include <QObject>
#include <QVector>
#include "../Node/networknode.h"
#include "../PortBindingManager/PortBindingManager.h"

class TopologyBuilder : public QObject
{
    Q_OBJECT

public:
    explicit TopologyBuilder(QObject *parent = nullptr);
    QVector<Router*> createMeshTopology(PortBindingManager *&portBindingManager, int asId, int dhcp, QSet<int> brokens, int firstNode, int nodeCount, const QString &routing);
    QVector<Router*> createRingstarTopology(PortBindingManager *&portBindingManager, int asId, int dhcp, QSet<int> brokens, int firstNode, int nodeCount, const QString &routing);
private:
Q_SIGNALS:
};

#endif    // TOPOLOGYBUILDER_H
