#ifndef IP_HPP
#define IP_HPP

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QByteArray>
#include <QHostAddress>

class IP : public QObject
{
    Q_OBJECT

public:
    explicit IP(QObject *parent = nullptr);
    virtual ~IP() = default;

Q_SIGNALS:

protected:
    IP(QObject *parent, bool isIPv4); // For differentiating between IPv4 and IPv6 if needed
};

class IPv4 : public IP
{
    Q_OBJECT

public:
    explicit IPv4(QObject *parent = nullptr);
    explicit IPv4(const QString &ipString, QObject *parent = nullptr);
    explicit IPv4(uint64_t ipValue, QObject *parent = nullptr);
    ~IPv4() override;

    QString toString() const;
    uint64_t toNumeric() const;

private:
    uint64_t m_ipValue;
};

class IPv6 : public IP
{
    Q_OBJECT

public:
    explicit IPv6(QObject *parent = nullptr);
    explicit IPv6(const QString &ipString, QObject *parent = nullptr);
    ~IPv6() override;

    QString toString() const;

private:
    QString m_ipValue;
};

typedef QSharedPointer<IPv4> IPv4Ptr_t;
typedef QSharedPointer<IPv6> IPv6Ptr_t;

#endif // IP_HPP
