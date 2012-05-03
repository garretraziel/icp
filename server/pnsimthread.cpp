#include "pnsimthread.h"
#include <QDebug>
#include <QFile>
#include <QtNetwork>
#include <QtXml>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDataStream>
#include <QTextStream>

PNSimThread::PNSimThread(int socketDescriptor, QObject *parent) :
    QThread(parent),socketDescriptor(socketDescriptor)
{
    isLogged = false;
    usersFile = "./users.dat";
    logFile = "./log.dat";
}

void PNSimThread::run()
{
    QTcpSocket commSock;
    //commSock = new QTcpSocket(this);
    if (!commSock.setSocketDescriptor(socketDescriptor)) {
        emit error(commSock.error());
        return;
    }
    commSock.waitForConnected(-1);

    bool connected = true;

    quint16 block = 0;
    QDataStream in(&commSock);
    in.setVersion(QDataStream::Qt_4_0);

    while (connected) {
        while (commSock.bytesAvailable() < (int)sizeof(quint16)) {
            commSock.waitForReadyRead(-1);
            if (commSock.state() == QTcpSocket::UnconnectedState) {
                connected = false;
                break;
            }
        }
        if (!connected) break;
        in >> block;
        qDebug() << "velikost: " << block;
        while (commSock.bytesAvailable() < block) {
            commSock.waitForReadyRead(-1);
        }
        QString command;
        in >> command;
        QString message = "";
        if (!handleCommand(command,message)) {
            commSock.write(createMessage(message));
            commSock.disconnectFromHost();
        }
        if (message != "")
            commSock.write(createMessage(message));
    }
    qDebug() << "odpojeno";
}

bool PNSimThread::handleCommand(QString command, QString &message)
{
    qDebug() << command;
    QXmlStreamReader com(command);
    if (com.readNext() != QXmlStreamReader::StartDocument) {
        qCritical() << "Error: unknown command from client";
        message = "<err info=\"Unknown command\"/>";
        return false;
    }
    com.readNext();
    if (com.atEnd() || com.hasError()) {
        qCritical() << "Error: bad command from client";
        message = "<err info=\"Bad command\"/>";
        return false;
    }
    QString strcmd = com.name().toString();
    if (strcmd == "register") {
        //todo
    } else if (strcmd == "login") {
        QString name = com.attributes().value("name").toString();
        QString password = com.attributes().value("password").toString();
        if (name == "" || password == "" || !logUser(name, password)) {
            qDebug() << "nemuzu lognout";
            message = "<err info=\"Bad password or user doesn't exist\"/>";
            return false;
        } else {
            isLogged = true;
            message = "<ok/>";
            qDebug() << "zalogovan";
        }
    } else if (!isLogged) {
        message = "<err info=\"Not logged on\"/>";
        qDebug() << "not logged!";
        return false;
    }
    return true;
}

bool PNSimThread::logUser(QString login, QString password)
{
    QFile users(usersFile);

    if (!users.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("Error: cannot open file with users");
        return false;
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login && items[1] == password) return true;
    }

    return false;
}

QByteArray PNSimThread::createMessage(QString message)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    return block;
}
