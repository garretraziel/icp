#include <QtCore/QCoreApplication>
#include "petrinetserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    PetriNetServer *server = new PetriNetServer(0,50,"127.0.0.1",11899);

    bool is_running = server->start();

    if (!is_running) {
        qCritical() << "Cannot start server\n";
        return 1; //todo: chyby nejak lip
    }

    return a.exec();
}
