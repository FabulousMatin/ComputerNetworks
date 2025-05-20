#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QtGlobal>
#include <cmath>
#include <QRandomGenerator>
#include "../Packet/packet.h"


class DataGenerator : public QObject
{
    Q_OBJECT

public:
    explicit DataGenerator(QList<int> pcIds, QObject *parent = nullptr);
    static int samplePareto(double x_min, double alpha);
    static int samplePoisson(double lambda);
    int getTotalPackets() const;
    void generateBuffers(QList<int> pcIds);



    QList<QList<PacketPtr_t>> getBuffers();

private:
    void generateBuffers();
    QVector<int> m_poissonDistribution;
    QVector<int> m_paretoDistribution;
    int m_cycles;
    int m_totalData;
    double m_xMin = 0;
    double m_alpha = 1.0;
    double m_lambda = 4.0;
    QString m_distribution;
    QList<QList<PacketPtr_t>> m_buffers;
    QList<QList<PacketPtr_t>> splitFileToPackets(const QString &filePath, int packetSize, int numParts);
Q_SIGNALS:
};

#endif    // DATAGENERATOR_H
