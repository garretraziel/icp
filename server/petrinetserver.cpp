#include "petrinetserver.h"
#include <QNetworkProxy>
#include <iostream>
#include <QByteArray>
#include <QtNetwork>

PetriNetServer::PetriNetServer(QObject *parent, int maxconnections, int port) :
    QTcpServer(parent)
{
    setProxy(QNetworkProxy::NoProxy);
    setMaxPendingConnections(maxconnections);

    /*QString ip;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, ipAddressesList) {
        if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
            ip = address.toString();
            break;
        }
    }*/
    my_ip = QHostAddress::Any;

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
        qDebug() << "listening on port: " << serverPort();
        qDebug() << "and ip:" << my_ip.toString();
    }
    return true;
}

void PetriNetServer::incomingConnection(int socketDescriptor)
{
    PNSimThread *thread = new PNSimThread(socketDescriptor, this);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));

    thread->start();
}
