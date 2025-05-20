#include "datagenerator.h"
#include <random>
#include <QFile>

QList<QList<PacketPtr_t>> DataGenerator::splitFileToPackets(const QString &filePath, int packetSize, int numParts)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << Q_FUNC_INFO << "DataGenerator: Unable to open file:" << filePath;
        qFatal("Unable to open file");
        return {};
    }

    QByteArray data = file.readAll();
    file.close();

    qInfo() << "DataGenerator: Read" << data.size() << "bytes from file:" << filePath;

    QList<PacketPtr_t> packets;
    int packetNum = 0;
    for(qint64 i = 0; i < data.size(); i += packetSize)
    {
        QByteArray payload = data.mid(i, packetSize);
        QSharedPointer<Packet> packet = Packet::createDataPacket(payload);
        packet->setTcpDataSeq(packetNum);
        packetNum++;
        packets.append(packet);
    }

    // 22828
    qInfo() << "Total packets:" << packetNum + 1;

    int partSize = packets.size() / numParts;
    QList<QList<QSharedPointer<Packet>>> parts;
    for(int i = 0; i < numParts; ++i)
    {
        QList<QSharedPointer<Packet>> part = packets.mid(i * partSize, partSize);
        parts.append(part);
    }

    int remaining = packets.size() % numParts;
    for(int i = 0; i < remaining; ++i)
    {
        parts[i].append(packets[numParts * partSize + i]);
    }

    return parts;
}

void DataGenerator::generateBuffers(QList<int> pcIds) {

    QString filePath = "D:\\ComputerNetworks\\CN_CHomeworks_3\\assets\\Taasiaan.mp3";
    size_t packetSize = 1'024;    // 1KB
    int pcCount = pcIds.size();
    QList<QList<PacketPtr_t>> chunks = splitFileToPackets(filePath, packetSize, pcCount);

    for(int i = 0; i < pcCount; ++i) {
        for(int j = 0; j < chunks[i].size(); j++) {
            if(pcIds[i]) {
                chunks[i][j]->setSequenceNumber(j);
            }
        }
    }

    m_buffers = chunks;
}


int DataGenerator::getTotalPackets() const {
    return m_totalData;
}

DataGenerator::DataGenerator(QList<int> pcIds, QObject *parent) :
    QObject {parent}
{
    generateBuffers(pcIds);
}

QList<QList<PacketPtr_t>> DataGenerator::getBuffers() {
    return m_buffers;
}


int DataGenerator::samplePoisson(double lambda) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::poisson_distribution<> dist(lambda);

    return dist(gen);
}


int DataGenerator::samplePareto(double x_min, double alpha) {
    double U = static_cast<double>(QRandomGenerator::global()->generate() / RAND_MAX);
    if (U == 0) U = 1e-10;

    return static_cast<int>(x_min / std::pow(1.0 - U, 1.0 / alpha));
}
