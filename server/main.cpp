#include <QtCore/QCoreApplication>
#include "petrinetserver.h"
#include "petrisim.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    PetriNetServer server(0,50,11899);

    bool is_running = server.start();

    if (!is_running) {
        qCritical() << "Cannot start server\n";
        return 1;
    }

    return a.exec();
}
