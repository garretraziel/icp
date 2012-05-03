#include "communicator.h"
#include <QDataStream>
#include <QByteArray>

#include <iostream>

Communicator communicator;

Communicator::Communicator()
{
    commSock = new QTcpSocket;
}

Communicator::~Communicator()
{
    qDebug() << "hurr";
    if (commSock->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "odpojuji";
        commSock->disconnectFromHost();
        //commSock->waitForDisconnected();
    }

    delete commSock;
}

bool Communicator::connect(QString hostname, QString port)
{
    commSock->connectToHost(hostname, port.toUInt(),QIODevice::ReadWrite);
    return true;
}

bool Communicator::sendCommand(QString command)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << command;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    qDebug() << "velikost: " << (quint16)(block.size() - sizeof(quint16));
    commSock->write(block);
    return true;
}

bool Communicator::login(QString name, QString password)
{
    QString message = "<login name=\"";
    message += name;
    message += "\" password=\"";
    message += password;
    message += "\"/>";

    return sendCommand(message);
}

