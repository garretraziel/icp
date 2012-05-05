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
    out << (qint64)0;
    out << command;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    commSock->write(block);
    return true;
}

#define returnWhenTimeout commSock->waitForReadyRead(); \
                          if (commSock->state() == QTcpSocket::UnconnectedState) \
                            return

bool Communicator::recvCommand(QString &command)
{
    qint64 block;
    while (commSock->bytesAvailable() < (int)sizeof(qint64)) {
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

    if(isNotError(recMessage, message)){
        loginName = name;
        return true;
    }
    else {
        loginName = "";
        return false;
    }
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

bool Communicator::isNotError(QString & recMessage, QString & message){
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

bool Communicator::saveSimState(QString xmlSimState, QString & message){
    //TODO!!
    QString sendMessage =
            "<save-simul name=\"jmeno\" version=\"-1\">\n"
            +xmlSimState+
            "</save-simul>";

    //FIXYY
    sendCommand(sendMessage);

    QString recMessage;
    if (!recvCommand(recMessage)) {
        message = "Error: server didn't response";
        return false;
    }

    return isNotError(recMessage, message);
}

bool Communicator::getSimulations(QStringList &sims){
    QString message = "<list-simuls/>";

    if(!sendCommand(message)){
        return false;
    }

    QString recMessage;

    if (!recvCommand(recMessage)) {
        return false;
    }

    QXmlStreamReader xml(recMessage);

    if (!xml.readNext() != QXmlStreamReader::StartDocument) {
        return false;
    }

    xml.readNext();
    if (xml.atEnd() || xml.hasError()) {
        return false;
    }

    if (xml.name() != "simul-list") return false;

    while (!xml.atEnd()) {
        xml.readNext();
        QString result = xml.attributes().value("name").toString() +"\tv"+
                xml.attributes().value("version").toString() +"\tby "+
                xml.attributes().value("author").toString()+"\t("+
                xml.attributes().value("info").toString()+")";
        sims.push_back(result);
    }

    return true;
}





