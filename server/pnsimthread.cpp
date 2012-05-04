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
        QString name = com.attributes().value("name").toString();
        QString password = com.attributes().value("password").toString();
        if (name == "" || password == "") {
            qDebug() << "nemuzu lognout";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        int result = registerUser(name,password);
        if (result != 0) {
            if (result == 1) {
                qDebug() << "already registere";
                message = "<err info=\"User is already registered\"/>";
                return false;
            } else if (result == 2) {
                qDebug() << "spatny soubor";
                message = "<err info=\"Cannot read usersfile, probably server's problem\"/>";
                return false;
            } else {
                qDebug() << "asi obsahuje dvojtecku";
                message = "<err info=\"Bad login or password\"/>";
                return false;
            }
        } else {
            isLogged = true;
            message = "<ok/>";
            qDebug() << "zalogovat, zaregistrovan";
        }
    } else if (strcmd == "login") {
        QString name = com.attributes().value("name").toString();
        QString password = com.attributes().value("password").toString();
        if (name == "" || password == "") {
            qDebug() << "nemuzu lognout";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        int result = logUser(name, password);
        if (result != 0) {
            if (result == 1) {
                qDebug() << "doesn't exists";
                message = "<err info=\"User doesn't exists\"/>";
                return false;
            } else if (result == 2) {
                qDebug() << "bad password";
                message = "<err info=\"Bad password\"/>";
                return false;
            } else {
                qDebug() << "file with users doesn't exists";
                message = "<err info=\"User file doesn't exists\"/>";
                return false;
            }
        } else {
            isLogged = true;
            message = "<ok/>";
            qDebug() << "zalogovan";
        }
    } else if (!isLogged) {
        message = "<err info=\"Not logged on\"/>";
        qDebug() << "not logged!";
        return false;
    } else {

    }
    return true;
}

int PNSimThread::logUser(QString login, QString password)
{
    QFile users(usersFile);

    if (!users.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("Error: cannot open file with users");
        return 3;
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login && items[1] == password) return 0;
        if (items[0] == login) return 2;
    }

    return 1;
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

int PNSimThread::registerUser(QString login, QString password)
{
    if (login == "" || password == "" || login.count(':') != 0 || password.count(':') != 0) {
        return 3;
    }

    QFile users(usersFile);

    if (!users.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qCritical("Error: cannot open file with users");
        return 2;
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login) return 1;
    }
    filestream << login << ":" << password << endl;
    return 0;
}
