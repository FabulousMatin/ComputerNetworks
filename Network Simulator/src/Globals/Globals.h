#ifndef GLOBALS_H
#define GLOBALS_H

namespace NETWORK
{


enum class State {
    IDLE,
    DHCP,
    ROUTING,
    SIMULATION,
    ANALYZE,
    END
};

enum class IPVersion
{
    IPv4,
    IPv6
};

enum class PacketType
{
    Data,
    Control
};

enum class PacketControlType
{
    Request,
    Response,
    Acknowledge,
    Error,
    DHCPDiscovery,
    DHCPOffer,
    DHCPRequest,
    DHCPAcknowledge,
    DHCPNak,
    RIP,
    OSPFHello,
    OSPFLsa,
    TCPAck
};

enum class DistributionType
{
    Poisson,
    Pareto
};

enum class TopologyType
{
    Mesh,
    RingStar,
    Torus
};
}    // namespace UT

#endif    // GLOBALS_H
