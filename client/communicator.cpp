#include "communicator.h"
#include <QDataStream>
#include <QByteArray>
#include <QMessageBox>
#include <QDebug>
#include <QXmlStreamReader>
#include <QDataStream>

Communicator communicator;

Communicator::Communicator(QWidget *parent):QObject(parent)
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
        QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
        block = 0;
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

bool Communicator::getSimulations(simList &sims){
    QString message = "<list-simuls/>";
    QObject::disconnect(this, SLOT(handleIncomming()));
    if(!sendCommand(message)){
        QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
        return false;
    }
    QString recMessage;

    if (!recvCommand(recMessage)) {
        QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
        return false;
    }
    QXmlStreamReader xml(recMessage);
    if (xml.readNext() != QXmlStreamReader::StartDocument) {
        QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
        return false;
    }
    xml.readNext();
    if (xml.atEnd() || xml.hasError()) {
        QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
        return false;
    }
    if (xml.name() != "simul-list"){
        QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
        return false;
    }

    while (!xml.atEnd()) {
        if (xml.readNextStartElement()) {
            StringVector list;
            list.push_back(xml.attributes().value("name").toString());
            list.push_back(xml.attributes().value("version").toString());
            list.push_back(xml.attributes().value("author").toString());
            list.push_back(xml.attributes().value("info").toString());


            sims.push_back(list);
        }
    }
    QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
    return true;
}

bool Communicator::loadThis(QString name, QString version){
    QString command;
    QXmlStreamWriter xml(&command);
    xml.writeStartDocument();
    xml.writeEmptyElement("simul-that");
    xml.writeAttribute("name",name);
    xml.writeAttribute("version",version);
    xml.writeEndDocument();
    return sendCommand(command);
}

bool Communicator::handleCommand(QString command){

    //neco

    //neco

    //neco

    //jen test
    sim = command;
    emit simOk();
}


void Communicator::handleIncomming(){
    if(commSock->bytesAvailable() < (int)sizeof(qint64))
        return;

    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);

    in >> block;

    if(commSock->bytesAvailable() < block) return;

    QString command;
    in >> command;

    block = 0;
    if (!handleCommand(command)) {

        return;
    }
}
