#include <QtCore/QCoreApplication>
#include "petrinetserver.h"
#include "petrisim.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    PetriNetServer *server = new PetriNetServer(0,50,"127.0.0.1",11899);

    bool is_running = server->start();

    if (!is_running) {
        qCritical() << "Cannot start server\n";
        return 1; //todo: chyby nejak lip
    }

    QFile soubor("semafor.xml");

    if(!soubor.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("Cannot open file.");
        return 1;
    }

    QString str;
    QTextStream stream(&soubor);

    while (!stream.atEnd()) {
        str += stream.readLine();
    }

    soubor.close();

    PetriSim *simulace = new PetriSim;

    simulace->setState(str);

    qDebug() << simulace->getState();

    simulace->run();

    qDebug() << simulace->getState();

    //qDebug() << "trololo";

    delete simulace;
    delete server;

    return a.exec();
}
