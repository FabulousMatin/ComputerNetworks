TEMPLATE = app
QT += core
QT += network
CONFIG += lrelease
CONFIG += no_keywords
CONFIG += console
CONFIG += c++20

# INCLUDEPATH += $$PWD\Globals

SOURCES += \
    $$PWD/Buffer/buffer.cpp \
    # $$PWD/DHCPServer/DHCPServer.cpp \
    $$PWD/EventsCoordinator/EventsCoordinator.cpp \
    $$PWD/EventsCoordinator/datagenerator.cpp \
    $$PWD/Header/bgpheader.cpp \
    $$PWD/Header/datalinkheader.cpp \
    $$PWD/Header/ipheader.cpp \
    $$PWD/Header/tcpheader.cpp \
    $$PWD/IP/IP.cpp \
    $$PWD/MAC/macaddress.cpp \
    $$PWD/MAC/macaddressgenerator.cpp \
    $$PWD/Node/network.cpp \
    $$PWD/Node/networknode.cpp \
    $$PWD/Packet/packet.cpp \
    $$PWD/PortBindingManager/PortBindingManager.cpp \
    $$PWD/Port/Port.cpp \
    $$PWD/Node/autonomoussystem.cpp \
    $$PWD/Routing/routingprotocol.cpp \
    $$PWD/TCP/tcpnewreno.cpp \
    $$PWD/Topology/topologybuilder.cpp \
    $$PWD/Utils/utils.cpp \
    $$PWD/applicationcontext.cpp \
    $$PWD/main.cpp \
    $$PWD/simulator.cpp

HEADERS += \
    $$PWD/Buffer/buffer.h \
    # $$PWD/DHCPServer/DHCPServer.h \
    $$PWD/EventsCoordinator/EventsCoordinator.h \
    $$PWD/EventsCoordinator/datagenerator.h \
    $$PWD/Globals/Globals.h \
    $$PWD/Header/bgpheader.h \
    $$PWD/Header/datalinkheader.h \
    $$PWD/Header/ipheader.h \
    $$PWD/Header/tcpheader.h \
    $$PWD/IP/IP.h \
    $$PWD/MAC/macaddress.h \
    $$PWD/MAC/macaddressgenerator.h \
    $$PWD/Node/network.h \
    $$PWD/Node/networknode.h \
    $$PWD/Packet/packet.h \
    $$PWD/PortBindingManager/PortBindingManager.h \
    $$PWD/Port/Port.h \
    $$PWD/Node/autonomoussystem.h \
    $$PWD/Routing/routingprotocol.h \
    $$PWD/TCP/tcpnewreno.h \
    $$PWD/Topology/topologybuilder.h \
    $$PWD/Utils/utils.h \
    $$PWD/applicationcontext.h \
    $$PWD/simulator.h
