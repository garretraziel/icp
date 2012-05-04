#include "petrinetserver.h"
#include <QNetworkProxy>
#include <iostream>
#include <QByteArray>

PetriNetServer::PetriNetServer(QObject *parent, int maxconnections, QString ip, int port) :
    QTcpServer(parent)
{
    setProxy(QNetworkProxy::NoProxy);
    setMaxPendingConnections(maxconnections);

    if (!my_ip.setAddress(ip)) {
        qCritical() << "Cannot set ip: " << ip;
    }

    this -> port = port; //pokud je port 0, automaticky vyber portu. Asi to neni spravne.

}

PetriNetServer::~PetriNetServer()
{ //todo: mozna odchytavat signal?
    qDebug() << "Ending server";
    close();
}

bool PetriNetServer::start()
{
    bool listening = listen(my_ip, port);

    if (!listening) {
        qCritical() << "Cannot listen on: " << port;
        return false;
    } else {
        qDebug() << "listening on: " << serverPort();
    }
    return true;
}

void PetriNetServer::incomingConnection(int socketDescriptor)
{
    PNSimThread *thread = new PNSimThread(socketDescriptor, this);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
}
