#include "petrinetserver.h"
#include <QNetworkProxy>

PetriNetServer::PetriNetServer(QObject *parent, int maxconnections, QString ip, int port) :
    QTcpServer(parent)
{
    setProxy(QNetworkProxy::NoProxy);
    setMaxPendingConnections(maxconnections);

    if (!my_ip.setAddress(ip)) {
        qCritical() << "Cannot set ip: " << ip;
    }

    this -> port = port; //pokud je port 0, automaticky vyber portu. Asi to neni spravne.

    connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}

PetriNetServer::~PetriNetServer() { //todo: mozna odchytavat signal?
    qDebug() << "Ending server";
    close();
}

bool PetriNetServer::start() {
    bool listening = listen(my_ip, port);

    if (not listening) {
        qCritical() << "Cannot listen on: " << port;
        return false;
    } else {
        qDebug() << "listening on: " << serverPort();
    }
    return true;
}

void PetriNetServer::handleNewConnection() {
    qDebug() << "client connected";

    QTcpSocket *socket = nextPendingConnection();
    if (!socket) return;

    connect(socket, SIGNAL(readyRead()), SLOT(communicate()));
}

void PetriNetServer::communicate() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(this->sender());
}
