/**
 * @file petrinetserver.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida, ktera poskytuje rozhrani TCP serveru. Pro kazdeho noveho uzivatele vytvori novy thread.
 */

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

    my_ip = QHostAddress::Any;

    this -> port = port; //pokud je port 0, automaticky vyber portu. Asi to neni spravne.
    iomutex = new QMutex;
}

PetriNetServer::~PetriNetServer()
{
    qDebug() << "Ending server";
    delete iomutex;
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
    PNSimThread *thread = new PNSimThread(socketDescriptor, iomutex, this);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
}
