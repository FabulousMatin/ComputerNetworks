#ifndef BGPHEADER_H
#define BGPHEADER_H

#include <QObject>

class BGPHeader : public QObject
{
    Q_OBJECT

public:
    explicit BGPHeader(QObject *parent = nullptr);

Q_SIGNALS:
};

#endif    // BGPHEADER_H
