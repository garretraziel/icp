#include "communicator.h"
#include <QDataStream>
#include <QByteArray>
#include <QMessageBox>
#include <QDebug>
#include <QXmlStreamReader>

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

#define returnWhenTimeout commSock->waitForReadyRead(); \
                          if (commSock->state() == QTcpSocket::UnconnectedState) \
                            return

bool Communicator::recvCommand(QString &command)
{
    quint16 block;
    while (commSock->bytesAvailable() < (int)sizeof(quint16)) {
        returnWhenTimeout false;
    }
    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);

    in >> block;
    qDebug() << "velikost: " << block;

    while (commSock->bytesAvailable() < block) {
        returnWhenTimeout false;
    }
    in >> command;

    return true;
}

inline bool Communicator::login_or_register(QString what, QString name, QString password, QString &message)
{
    QString sendMessage = "<";
    sendMessage += what;
    sendMessage += " name=\"";
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

    QXmlStreamReader xml(recMessage);

    if (xml.readNext() != QXmlStreamReader::StartDocument) {
        message = "Server talks rubbishly.";
        return false;
    }
    xml.readNext();
    if (xml.atEnd() || xml.hasError()) {
        message = "Server talks rubbishly.";
        return false;
    }
    if (xml.name() == "err") {
        message = xml.attributes().value("info").toString();
        return false;
    }

    return true;
}

bool Communicator::login(QString name, QString password, QString &message)
{
    return login_or_register("login",name,password,message);
}

bool Communicator::registerUser(QString name, QString password, QString &message)
{
    return login_or_register("register",name,password,message);
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

void Communicator::sendSimState(QString xmlSimState){
    //TODO!!
    QString sendMessage = "<xml hurr durr>\n"+xmlSimState+"</xml hurr durr>";
    sendCommand(sendMessage);


}


