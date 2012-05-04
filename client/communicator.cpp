#include "communicator.h"
#include <QDataStream>
#include <QByteArray>
#include <QMessageBox>
#include <QDebug>

#include <iostream>

Communicator communicator;

Communicator::Communicator()
{
    commSock = new QTcpSocket;
}

Communicator::~Communicator()
{
    if (connected()) {
        commSock->disconnectFromHost();
    }

    delete commSock;
}

bool Communicator::connected()
{
    if (commSock->state() == QAbstractSocket::ConnectedState) {
        return true;
    } else {
        return false;
    }
}

bool Communicator::connect(QString hostname, QString port)
{
    commSock->connectToHost(hostname, port.toUInt(),QIODevice::ReadWrite);
    commSock->waitForConnected(-1);
    return connected();
}

bool Communicator::sendCommand(QString command)
{
    if (!connected()) return false;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << command;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    commSock->write(block);
    return true;
}

bool Communicator::recvCommand(QString &command)
{
    return false;
}

bool Communicator::login(QString name, QString password, QString &message)
{
    QString sendMessage = "<login name=\"";
    sendMessage += name;
    sendMessage += "\" password=\"";
    sendMessage += password;
    sendMessage += "\"/>";

    if (!sendCommand(sendMessage)) {
        message = "Error: cannot send message to server";
        return false;
    }
    QString recMessage;
    if (!recvCommand(recMessage)) {
        message = "Error: server didn't response";
        return false;
    }
}

void Communicator::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(NULL, "Connection refused","Connection was refused by the server. Make sure that server is running.");
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(NULL, "Host not found","Hostname was not found. Check hostname and port again.");
        break;
    default:
        QMessageBox::information(NULL, "Error", "Error occured during connecting to server.");
        break;
    }
}

bool Communicator::registerUser(QString name, QString password, QString &message)
{
    QString sendMessage = "<register name=\"";
    sendMessage += name;
    sendMessage += "\" password\"";
    sendMessage += password;
    sendMessage += "\"/>";

    if (!sendCommand(sendMessage)) {
        message = "Error: cannot connect or send message";
        return false;
    }
    QString recMessage;
    if (!recvCommand(recMessage)) {
        message = "Error: server didn't response";
        return false;
    }
}
