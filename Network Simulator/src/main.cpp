#include <QObject>
#include <QCoreApplication>
#include "applicationcontext.h"
#include <QVector>
#include "Node/network.h"
#include "EventsCoordinator/EventsCoordinator.h"
#include "simulator.h"


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    ApplicationContext *appContext = new ApplicationContext();
    Simulator *simulator = new Simulator(15, 23);

    EventsCoordinator *eventCoordinator = EventsCoordinator::instance(
        Millis(appContext->config.cycleDuration),
        appContext->config.simulationDuration / appContext->config.cycleDuration
    );
    eventCoordinator->createDataGenerator(appContext->config.statisticalDistribution, appContext->config.packetsPerSimulation);


    Network* network = new Network(appContext->config, eventCoordinator, simulator);

    QObject::connect(simulator, &Simulator::changeState, eventCoordinator, &EventsCoordinator::stateChanged);
    QObject::connect(eventCoordinator, &EventsCoordinator::simulationEnd, simulator, &Simulator::simulationDone);
    simulator->begin();

    return app.exec();
}
